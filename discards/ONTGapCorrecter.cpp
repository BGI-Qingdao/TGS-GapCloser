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
#include "biocommon/paf/PAF.h"
#include "biocommon/seq/tool_func.h"

#include "appcommon/contigPairInfo.h"
#include "appcommon/ScaffInfo.h"
#include "appcommon/ONT2Gap.h"

#include <map>
#include <vector>
#include <set>
#include <algorithm>


typedef BGIQD::FASTQ::Id_Desc_Head Header;

typedef BGIQD::FASTQ::Fastq<Header> ONTRead;

typedef BGIQD::FASTQ::FastqReader<ONTRead> Reader ;

typedef std::vector<BGIQD::PAF::PAF_Item>  Contig2ONT;

typedef std::map<std::string , Contig2ONT> Contig2ONTReads;

struct AppConfig
{

    std::string  contig_2_ont_paf_file ;

    std::map<std::string , ONTRead> reads ;

    std::map<unsigned int, Contig2ONTReads> aligned_data;

    BGIQD::stLFR::ScaffInfoHelper scaff_info_helper;

    BGIQD::LOG::logger loger;

    void Init()
    {
        BGIQD::LOG::logfilter::singleton().
            get("ONTGapFiller",BGIQD::LOG::loglevel::INFO, loger);
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
                                //chooses.push_back(std::make_pair( m1 , m2 ));
                                chooses.push_back( { m1 , m2 , ont_pn } );
                            }
                            else
                            {
                                if( force_fill )
                                    others.push_back( { m1 , m2 , ont_pn } );
                                    //others.push_back(std::make_pair( m1 , m2 ));
                            }
                        }
                    }
                    if( used_pair > 0 )
                        used_read ++ ;
                    a_read_oo_choose_freq.Touch(used_pair);
                }
                gap_oo_read_freq.Touch(used_read);
                filler_choose_freq.Touch(chooses.size());
                if( work_mode == 1 )
                {
                    BGIQD::ONT::SortModifyLess(chooses);
                }
                else if ( work_mode == 3 )
                {
                    BGIQD::ONT::SortMedian(chooses);
                }
                if( chooses.empty() )
                {
                    no_choose ++ ;
                    if(  force_fill )
                    {
                        // trust contig overlap first 
                        if( scaff_pn.gap_size < 0 )
                        {
                            scaff_negotive_gap_size_no_choose ++ ;
                        }
                        else
                        {
                            // force choose some to fill ;
                            for ( const auto & pair : others )
                            {
                                auto & m1 = pair.from ;
                                auto & m2 = pair.to;
                                BGIQD::stLFR::PairPN tmp ;
                                tmp.InitFromPAF(m1,m2);
                                if( tmp.type == BGIQD::stLFR::OOType::Unknow  )
                                    continue ;
                                if( tmp.gap_size < 0 )
                                {
                                    prev.gap_size = tmp.gap_size ;
                                    ont_negotive_gap_size_no_choose ++ ;
                                    break ;
                                }
                                else if ( tmp.gap_size == 0 )
                                {
                                    prev.gap_size = -1;
                                    ont_negotive_gap_size_no_choose ++ ;
                                    break ;
                                }
                                else
                                {
                                    std::vector<int> pos;
                                    pos.push_back(m1.target_start);
                                    pos.push_back(m2.target_start);
                                    pos.push_back(m1.target_end);
                                    pos.push_back(m2.target_end);
                                    std::sort( pos.begin() , pos.end()) ;
                                    int cut_start = pos[1] + 1 ;
                                    int cut_len = pos[2] - pos[1] - 1 ;
                                    //const auto & ont_read = reads.at(m1.target_name).seq.atcgs ;
                                    prev.gap_size = cut_len ;
                                    //if( cut_len < 1 || cut_start<0 ||  cut_start + cut_len  >= (int) ont_read.size() )
                                    //{
                                    //    cut_err ++ ;
                                    //    continue ;
                                    //}
                                    force_fill_succ ++ ;
                                    prev.gap_size = cut_len ;
                                    break ;
                                }
                            }
                        }
                    }
                        continue ;
                }
                // Random choose the 1th 1 choose .
                for ( const auto & pair : chooses )
                {
                    //auto & pair = chooses.front() ;
                    auto & m1 = pair.from ;
                    auto & m2 = pair.to;
                    BGIQD::stLFR::PairPN tmp ;
                    tmp.InitFromPAF(m1,m2);

                    // trust contig overlap first 
                    if( scaff_pn.gap_size < 0 )
                    {
                        scaff_negotive_gap_size ++ ;
                        break;
                    }
                    if( tmp.gap_size < 0 )
                    {
                        ont_negotive_gap_size ++ ;
                        prev.gap_size = tmp.gap_size ;
                        break;
                    }
                    if( tmp.gap_size == 0 )
                    {
                        ont_negotive_gap_size ++ ;
                        prev.gap_size = -1 ;
                        break ;
                    }
                    int cut_start = 0 ;
                    int cut_len = tmp.gap_size ;
                    bool need_reverse = false ;
                    if( ( m1.query_char == '+' && prev.orientation)
                    ||  ( m1.query_char == '-' && !prev.orientation ) )
                    {
                        cut_start = m1.target_end + 1 ;
                    }
                    else
                    {
                        cut_start = m2.target_end + 1;
                        need_reverse = true ;
                    }
                    //const auto & ont_read = reads.at(m1.target_name).seq.atcgs ;
                    //if( cut_start<0 ||  cut_start + cut_len  >= (int) ont_read.size() )
                    //{
                    //    cut_err ++ ;
                    //    continue ;
                    //}
                    prev.gap_size = cut_len ;
                    break ;
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

        if( force_fill )
        {
            loger<<BGIQD::LOG::lstart()<<">the scaff_negotive_gap_size_no_choose  is "
                <<scaff_negotive_gap_size_no_choose<<BGIQD::LOG::lend();

            loger<<BGIQD::LOG::lstart()<<">the force fill succ is "
                <<force_fill_succ<<BGIQD::LOG::lend();
            loger<<BGIQD::LOG::lstart()<<">the ont_negotive_gap_size_no_choose is "
                <<ont_negotive_gap_size_no_choose<<BGIQD::LOG::lend();
        }

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

    bool force_fill ;

    int  work_mode ;

} config ;

int main(int argc , char ** argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string, contig2ont_paf ,"the paf file that map contig into ont reads.");
        DEFINE_ARG_OPTIONAL(bool, force_fill,"will force fill as much gap as it can. ","false");
        DEFINE_ARG_OPTIONAL(int, work_mode,"1, shortest ; 2, random ; 3, median","1");
    END_PARSE_ARGS;

    config.force_fill = force_fill.to_bool() ;

    config.work_mode = work_mode.to_int() ;

    config.contig_2_ont_paf_file = contig2ont_paf.to_string() ;

    config.Init();

    BGIQD::LOG::timer t(config.loger,"ONTGapFiller");

    config.LoadPAF() ;

    config.LoadScaffInfo();

    config.ParseAllGap();

    config.PrintScaffInfo() ;

    return 0 ;
}

