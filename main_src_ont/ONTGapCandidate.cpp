#include "common/args/argsparser.h"
#include "common/log/log.h"
#include "common/log/logfilter.h"
#include "common/files/file_writer.h"
#include "common/files/file_reader.h"
#include "common/string/stringtools.h"
#include "common/error/Error.h"
#include "common/stl/mapHelper.h"
#include "common/freq/freq.h"

#include "biocommon/fastq/fastq.h"
#include "biocommon/fasta/fasta.h"
#include "biocommon/paf/PAF.h"
#include "biocommon/seq/tool_func.h"

#include "appcommon/contigPairInfo.h"
#include "appcommon/ScaffInfo.h"
#include "appcommon/ONT2Gap.h"

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>

typedef BGIQD::FASTQ::Id_Desc_Head QHeader;

typedef BGIQD::FASTQ::Fastq<QHeader> QONTRead;

typedef BGIQD::FASTQ::FastqReader<QONTRead> QReader ;

typedef BGIQD::FASTA::Id_Desc_Head AHeader;

typedef BGIQD::FASTA::Fasta<AHeader> AONTRead;

typedef BGIQD::FASTA::FastaReader<AONTRead> AReader ;

typedef BGIQD::SEQ::seq Seq;

typedef std::vector<BGIQD::PAF::PAF_Item>  Contig2ONT;

typedef std::map<std::string , Contig2ONT> Contig2ONTReads;

struct AppConfig
{

    std::string  contig_2_ont_paf_file ;

    std::map<std::string , Seq> reads ;

    std::map<unsigned int, Contig2ONTReads> aligned_data;

    BGIQD::stLFR::ScaffInfoHelper scaff_info_helper;

    BGIQD::LOG::logger loger;

    void Init()
    {
        BGIQD::LOG::logfilter::singleton().
            get("ONTGapFiller",BGIQD::LOG::loglevel::INFO, loger);
    }


    void LoadONTReads()
    {
        BGIQD::LOG::timer t(loger,"LoadONTReads");
        std::string  ont_reads_file = "";
        if(  ont_read_a.empty() )
        {
            ont_reads_file = ont_read_q ;
            auto in = BGIQD::FILES::FileReaderFactory
                ::GenerateReaderFromFileName(ont_reads_file);
            if ( in == NULL )
                FATAL(" failed to open the ont_read_q to read ");
            QReader reader ;
            QONTRead tmp ;
            while( reader.LoadNextFastq(*in ,tmp) )
            {
                reads[tmp.head.Id] = tmp.seq ;
            }
            delete in ;
        }
        else
        {
            ont_reads_file = ont_read_a ;
            auto in = BGIQD::FILES::FileReaderFactory
                ::GenerateReaderFromFileName(ont_reads_file);
            if ( in == NULL )
                FATAL(" failed to open the ont_read_a to read ");
            AReader reader ;
            AONTRead tmp ;
            while( reader.LoadNextFasta(*in ,tmp) )
            {
                reads[tmp.head.Id] = tmp.seq ;
            }
            delete in ;
        }
    }

    void LoadPAF()
    {
        BGIQD::LOG::timer t(loger,"LoadPAF");
        auto in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName( contig_2_ont_paf_file  );
        if ( in == NULL )
            FATAL(" failed to open the contig_2_ont_paf_file to read ");

        BGIQD::PAF::PAF_Item tmp ;
        std::string line ;
        std::map<std::string , std::set<unsigned int >> read_2_contig ;
        while( ! std::getline( *in , line ).eof()  )
        {
            tmp.InitFromString(line);
            //if( tmp.len_target_match <= 0 
            //        || tmp.IDY() < 0.8 )
            //    continue ;
            unsigned int contig = std::stoul( tmp.query_name );
            aligned_data[contig][tmp.target_name].push_back(tmp);
            read_2_contig[tmp.target_name].insert(contig);
        }
        delete in ;

        BGIQD::FREQ::Freq<int>  contig2read_num_freq ;
        BGIQD::FREQ::Freq<int>  contig2a_read_freq;
        BGIQD::FREQ::Freq<int>  read2contig_freq;

        for( const auto & pair : read_2_contig )
        {
            read2contig_freq.Touch(pair.second.size());
        }
        for( const auto & pair1 : aligned_data )
        {
            contig2read_num_freq.Touch(pair1.second.size());
            for( const auto & pair2 : pair1.second)
            {
                contig2a_read_freq.Touch(pair2.second.size());
            }
        }
        loger<<BGIQD::LOG::lstart()<<">the read2contig freq is \n"
            <<read2contig_freq.ToString()<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the contig2read_num_freq freq is \n"
            <<contig2read_num_freq.ToString()<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the contig2a_read_freq freq is \n"
            <<contig2a_read_freq.ToString()<<BGIQD::LOG::lend();
    }

    void LoadScaffInfo()
    {
        BGIQD::LOG::timer t(loger,"LoadScaffInfo");
        scaff_info_helper.LoadAllScaff(std::cin) ;
    }
    static int myrandom (int i) { return std::rand()%i;}
    void ParseAllGap()
    {
        BGIQD::LOG::timer t(loger,"ParseAllGap");

        int gap_tatal = 0 ;
        int no_common = 0 ;
        int no_choose = 0 ;
        int no_match = 0 ;
        int cut_err = 0 ;
        int scaff_negotive_gap_size = 0 ;
        int scaff_negotive_gap_size_no_choose = 0 ;
        int ont_negotive_gap_size = 0 ;
        int ont_negotive_gap_size_no_choose = 0 ;
        int force_fill_succ = 0 ;

        BGIQD::FREQ::Freq<int> gap_both_read_freq ;
        BGIQD::FREQ::Freq<int> gap_oo_read_freq ;
        BGIQD::FREQ::Freq<int> filler_choose_freq ;
        BGIQD::FREQ::Freq<int> a_read_oo_choose_freq ;

        int gap_id = 0 ;
        for( auto & pair : scaff_info_helper.all_scaff )
        {
            auto & a_scaff = pair.second.a_scaff ;
            for( int i = 1 ; i < (int) a_scaff.size() ; i++ )
            {
                gap_tatal ++ ;
                auto & prev = a_scaff.at(i-1);
                auto & next = a_scaff.at(i) ;
                auto itr1 = aligned_data.find(prev.contig_id) ;
                auto itr2 = aligned_data.find(next.contig_id) ;
                if( itr1 == aligned_data.end() || itr2 == aligned_data.end() )
                {
                    no_match ++ ;
                    continue ;
                }
                auto & map_info1 = itr1->second ;
                auto & map_info2 = itr2->second ;
                std::set<std::string> commons;

                for( const auto & pair : map_info1 )
                {
                    if( map_info2.find( pair.first) != map_info2.end() )
                        commons.insert(pair.first);
                }

                gap_both_read_freq.Touch(commons.size());
                if( commons.empty() )
                {
                    no_common ++ ;
                    continue;
                }
                gap_id = prev.contig_id ;
                BGIQD::stLFR::PairPN scaff_pn;
                scaff_pn.InitFromScaffInfo(prev,next);
                int used_read = 0 ;
                BGIQD::ONT::ONT2GapInfos chooses ;
                BGIQD::ONT::ONT2GapInfos others;
                //std::vector< std::pair< BGIQD::PAF::PAF_Item , BGIQD::PAF::PAF_Item> > chooses;
                //std::vector< std::pair< BGIQD::PAF::PAF_Item , BGIQD::PAF::PAF_Item> > others;
                for( const auto & read_name : commons )
                {
                    auto & prev_matched_infos = map_info1.at(read_name) ;
                    auto & next_matched_infos = map_info2.at(read_name) ;
                    int used_pair = 0 ;
                    for( const auto & m1  : prev_matched_infos )
                    {
                        for( const auto & m2 : next_matched_infos )
                        {
                            BGIQD::stLFR::PairPN ont_pn ;
                            ont_pn.InitFromPAF(m1,m2);
                            if( ont_pn.type == scaff_pn.type )
                            {
                                used_pair ++ ;
                                chooses.push_back({ m1 , m2 , ont_pn });
                                //chooses.push_back(std::make_pair( m1 , m2 ));
                            }
                            else
                            {
                            }
                        }
                    }
                    if( used_pair > 0 )
                        used_read ++ ;
                    a_read_oo_choose_freq.Touch(used_pair);
                }
                if( work_mode == 1 )
                {
                    BGIQD::ONT::SortModifyLess(chooses);
                    //BGIQD::ONT::SortLess(chooses);
                }
                else if ( work_mode == 3 )
                {
                    BGIQD::ONT::SortMedian(chooses);
                }
                else if ( work_mode == 2 )
                {
                    std::random_shuffle(chooses.begin() , chooses.end(),myrandom);
                    std::random_shuffle(chooses.begin() , chooses.end(),myrandom);
                }
                else
                {
                    assert(0);
                }

                gap_oo_read_freq.Touch(used_read);
                filler_choose_freq.Touch(chooses.size());
                if( chooses.empty() )
                {
                    no_choose ++ ;
                }
                for ( const auto & pair : chooses )
                {
                    //auto & pair = chooses.front() ;
                    auto  m1 = pair.from  ;
                    auto  m2 = pair.to ;
                    const BGIQD::stLFR::PairPN & tmp = pair.pair_info  ;
                    m1 = m1.Flatten();
                    m2 = m2.Flatten();
                    std::string fill = "N" ;

                    if (tmp.gap_size > 0 )
                    {
                        int cut_start = 0 ;
                        int cut_len = tmp.gap_size ;
                        bool need_reverse = false ;
                        if( ( m1.query_char == '+' && prev.orientation)
                                ||  ( m1.query_char == '-' && !prev.orientation ) )
                        {
                            cut_start = m1.target_end ;
                        }
                        else
                        {
                            cut_start = m2.target_end;
                            need_reverse = true ;
                        }
                        const auto & ont_read = reads.at(m1.target_name).atcgs ;
                        if( cut_start<0 ||  cut_start + cut_len  >= (int) ont_read.size() )
                        {
                            assert(0);
                        }
                        // expand 2K from left & right
                        int new_cut_start =cut_start - 2000 ;
                        if(new_cut_start <0 ) new_cut_start = 0 ;
                        cut_len += cut_start - new_cut_start ;
                        if( new_cut_start + cut_len + 2000 >= (int)ont_read.size() )
                        {
                            cut_len = ont_read.size() - new_cut_start -1 ;
                        }
                        else 
                            cut_len += 2000 ;
                        cut_start = new_cut_start;

                        std::string cut_seq =  ont_read.substr(cut_start,cut_len) ;
                        if( need_reverse )
                            cut_seq = BGIQD::SEQ::seqCompleteReverse(cut_seq);
                        fill = cut_seq;
                    }
                    else if ( tmp.gap_size < 0 && tmp.gap_size >= -2000 )
                    {
                        int cut_start = 0 ;
                        int cut_len = -tmp.gap_size ;
                        bool need_reverse = false ;
                        if( ( m1.query_char == '+' && prev.orientation)
                                ||  ( m1.query_char == '-' && !prev.orientation ) )
                        {
                            cut_start = m1.target_end ;
                        }
                        else
                        {
                            cut_start = m2.target_end;
                            need_reverse = true ;
                        }
                        cut_start = cut_start -cut_len +1  ;
                        const auto & ont_read = reads.at(m1.target_name).atcgs ;
                        //  ONT is too short.
                        if( cut_start < 0 || cut_start + cut_len >=(int) ont_read.size() )
                            continue ;
                        if ( cut_len < 2000 )
                        {
                            int append = ( 2000 -cut_len )/2;
                            if( cut_start > append )
                            {
                                cut_start = cut_start - append ;
                                cut_len = cut_len + append ;
                            }
                            else
                            {
                                cut_len = cut_len + cut_start ;
                                cut_start = 0 ;
                            }
                            if( cut_start + cut_len + append >= (int) ont_read.size() )
                            {
                                cut_len += (int) ont_read.size() - (cut_start + cut_len) ;
                            }
                            else
                            {
                                cut_len += append ;
                            }
                        }
                        if( cut_start<0 ||  cut_start + cut_len  >= (int) ont_read.size() )
                        {
                            assert(0);
                        }
                        fill  =  ont_read.substr(cut_start,cut_len) ;
                    }

                    std::cout<<gap_id<<'\t'
                        <<prev.gap_size<<'\t'
                        <<tmp.gap_size<<'\t'
                        <<m1.target_name<<'\t'
                        <<m1.target_len<<'\t'
                        <<m1.query_len << '\t'
                        <<float(m1.aligned_len) / float(m1.query_len)<<'\t'
                        <<float(m1.match_len) / float(m1.aligned_len)<<'\t'
                        <<m2.query_len << '\t'
                        <<float(m2.aligned_len) / float(m2.query_len)<<'\t'
                        <<float(m2.match_len) / float(m2.aligned_len)<<'\t'
                        <<fill
                        <<'\n'

                        ;
                }
            }
        }

        loger<<BGIQD::LOG::lstart()<<">the gap_total is "
            <<gap_tatal<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the no_match is "
            <<no_match<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the no choose is "
            <<no_choose<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the no common is "
            <<no_common<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the scaff_negotive_gap_size is "
            <<scaff_negotive_gap_size<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the ont_negotive_gap_size is "
            <<ont_negotive_gap_size<<BGIQD::LOG::lend();


        loger<<BGIQD::LOG::lstart()<<">the common reads count freq for a gap \n"
            <<gap_both_read_freq.ToString()<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the correct oo reads count freq for a gap \n"
            <<gap_oo_read_freq.ToString()<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the filler choose count freq for a gap \n"
            <<filler_choose_freq.ToString()<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the one read provide filler choose count freq for a gap \n"
            <<a_read_oo_choose_freq.ToString()<<BGIQD::LOG::lend();

        loger<<BGIQD::LOG::lstart()<<">the cut error is "
            <<cut_err<<BGIQD::LOG::lend();
    }

    void PrintScaffInfo()
    {
        BGIQD::LOG::timer t(loger,"PrintScaffInfo");
        scaff_info_helper.PrintAllScaff(std::cout);
    }

    std::string ont_read_a  ;
    std::string ont_read_q  ;
    int work_mode ;
} config ;

int main(int argc , char ** argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string, contig2ont_paf ,"the paf file that map contig into ont reads.");
        DEFINE_ARG_OPTIONAL(std::string, ont_reads_q,"the ont reads in fastq format.","");
        DEFINE_ARG_OPTIONAL(std::string, ont_reads_a,"the ont reads in fasta format.","");
        DEFINE_ARG_OPTIONAL(int, work_mode,"1, shortest ; 2, random ; 3, median","1");
    END_PARSE_ARGS;

    if( ! ont_reads_q.setted && ! ont_reads_a.setted )
        FATAL("please give the ont reads !!!");
    config.work_mode = work_mode.to_int();
    if( ont_reads_a.setted )
    {
        config.ont_read_a  = ont_reads_a.to_string();
        config.ont_read_q  = "" ;
    }
    else
    {
        config.ont_read_q  = ont_reads_q.to_string();
        config.ont_read_a  = "" ;
    }

    config.contig_2_ont_paf_file = contig2ont_paf.to_string() ;

    config.Init();
    srand (time(NULL));
    BGIQD::LOG::timer t(config.loger,"ONTGapFiller");

    config.LoadONTReads() ;

    config.LoadPAF() ;

    config.LoadScaffInfo();

    config.ParseAllGap();

    return 0 ;
}

