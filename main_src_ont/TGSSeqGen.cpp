#include "common/args/argsparser.h"
#include "common/log/log.h"
#include "common/log/logfilter.h"
#include "common/files/file_writer.h"
#include "common/files/file_reader.h"
#include "common/error/Error.h"

#include "biocommon/fasta/fasta.h"
#include "biocommon/seq/tool_func.h"

#include "appcommon/fileName.h"

#include "appcommon/ScaffInfo.h"

#include <map>
#include <set>

struct AppConfig
{
    BGIQD::SOAP2::FileNames fNames;

    BGIQD::LOG::logger loger;

    BGIQD::stLFR::ScaffInfoHelper scaff_helper;

    typedef BGIQD::FASTA::SOAP2ContigHead Header;

    typedef BGIQD::FASTA::Fasta<Header> ContigFasta;

    typedef BGIQD::FASTA::FastaReader<ContigFasta> Reader;

    std::map<unsigned int , ContigFasta> contigs;

    std::set<unsigned int> used;

    void Init( const std::string & prefix )
    {
        fNames.Init(prefix);
        BGIQD::LOG::logfilter::singleton().get("TGSSeqGen",BGIQD::LOG::loglevel::INFO, loger);
    }

    void LoadScaffInfos()
    {
        auto  in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName(fNames.updated_scaff_infos());
        if( in == NULL )
            FATAL("failed to open xxx.updated_scaff_infos to read");

        scaff_helper.LoadAllScaff(*in);
        delete in ;
    }

    void LoadContigs()
    {
        auto in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName(fNames.contig());
        if( in == NULL )
            FATAL("failed to open xxx.contig to read");

        Reader reader;
        ContigFasta tmp ;
        while( reader.LoadNextFasta(*in , tmp) )
        {
            contigs[tmp.head.contigId] = tmp ;
        }
        delete in ;
    }

    struct DiffDetail
    {
        char type ;
        int  now_s; /* 1base */ 
        int  now_e; /* 1base */
        int  old_s; /* 1base */
        int  old_e; /* 1base */
    };
    struct SeqDetail
    {
        std::string str;
        int nsize ;
        int fill_size ;
        int cut_size ;
        int seq_size ;
        int start_n ;
        void SetN(int i )
        {
            assert( i >= 0);
            cut_size = -1 ;
            fill_size = -1;
            nsize = i ;
        }
        void SetStartN( int i )
        {
            assert( i >= 0);
            start_n = i ;
        }
        void SetCut( int i )
        {
            assert( i >= 0);
            cut_size = i ;
            fill_size = -1 ;
            nsize = -1 ;
        }

        void SetFill( int i )
        {
            assert( i >= 0);
            cut_size = -1 ;
            fill_size = i ;
            nsize = -1 ;
        }
    };

    SeqDetail get_atcg( const BGIQD::stLFR::ContigDetail & detail )
    {
        SeqDetail ret;
        std::string str = contigs.at(detail.contig_id).seq.atcgs ;
        if( ! detail.orientation )
            str = BGIQD::SEQ::seqCompleteReverse(str);
        ret.SetStartN(0);
        if( detail.extra.find(BGIQD::stLFR::ContigDetail::PREV_N) != detail.extra.end() ) {
            int prev_n = std::stoi(detail.extra.at(BGIQD::stLFR::ContigDetail::PREV_N));
            if( prev_n > 0 ) {
                ret.SetStartN(prev_n);
                str = std::string(prev_n,'N') + str ;
            }
        }
        if( detail.gap_size > 0 )
        {
            ret.seq_size = str.size();
            if( detail.extra.find(BGIQD::stLFR::ContigDetail::ONT_FILL) 
                    != detail.extra.end() )
            {
                str += detail.extra.at(BGIQD::stLFR::ContigDetail::ONT_FILL);
                assert(detail.gap_size ==
                        detail.extra.at(
                            BGIQD::stLFR::ContigDetail::ONT_FILL
                            ).size());
                ret.SetFill(detail.gap_size);
            }
            else
            {
                str += std::string(detail.gap_size,'N');
                ret.SetN(detail.gap_size);
            }
        }
        else if ( detail.gap_size < 0 )
        {
            if(  (int)str.length() + detail.gap_size > 0 )
            {
                str = str.substr(0,str.length() + detail.gap_size);
                ret.seq_size = str.length() ;//+ detail.gap_size ;
            }
            else
            {
                str = "";
                ret.seq_size = 0 ; 
            };
            ret.SetCut(-detail.gap_size);
        }
        else
        {
            ret.seq_size = str.size();
            ret.SetN(0);
            // gap size = 0
            ;
        }

        ret.str = str;
        return ret ;
    };
    std::map< std::string , std::vector<DiffDetail> > all_details;

    void PrintScaffSeqs()
    {
        auto out = BGIQD::FILES::FileWriterFactory
            ::GenerateWriterFromFileName(fNames.scaff_seqs()) ;
        if( out == NULL )
            FATAL(" failed to open xxx.scaff_seqs for write !");

        for( const auto & pair : scaff_helper.all_scaff)
        {
            std::vector<DiffDetail> details ;
            std::string scaff_name ;
            if( name_map.find(pair.first) == name_map.end() )
                scaff_name = "scaffold_"+std::to_string(pair.first);
            else
                scaff_name = name_map.at(pair.first);
            (*out)<<'>'<<scaff_name<<'\n';
            std::string str ;
            int start_pos = 1 ;
            DiffDetail seq;
            for( const auto & i : pair.second.a_scaff )
            {
                used.insert(i.contig_id);
                auto ret =  get_atcg(i);
                if( ret.seq_size  == 0 )
                    continue ;
                str += ret.str ;
                // push seq info
                seq.type = 'S';
                seq.now_s = start_pos ;
                seq.now_e = start_pos + ret.seq_size -1 ;
                seq.old_s = i.start_pos ;
                if( ret.start_n > 0 ) {
                    seq.old_s -= ret.start_n ;
                    assert( seq.old_s > 0 ) ;
                }
                seq.old_e = seq.old_s + ret.seq_size -1 ;
                details.push_back(seq);
                start_pos = seq.now_e +1 ;
                if( ret.nsize > 0 )
                {
                    // push n info
                    DiffDetail ninfo ;
                    ninfo.type = 'N';
                    ninfo.now_s = start_pos ;
                    ninfo.now_e = ninfo.now_s + ret.nsize  -1 ;
                    details.push_back(ninfo);
                    start_pos = ninfo.now_e +1 ;
                }
                if( ret.fill_size> 0 )
                {
                    // push n info
                    DiffDetail finfo ;
                    finfo.type = 'F';
                    finfo.now_s = start_pos ;
                    finfo.now_e = finfo.now_s + ret.fill_size -1 ;
                    details.push_back(finfo);
                    start_pos = finfo.now_e +1 ;
                }
            }
            (*out)<<BGIQD::SEQ::blockSeq(str,100);
            all_details[scaff_name]=details;
        }
        delete out ;
    }

    void PrintDetails()
    {
        auto detail_f= BGIQD::FILES::FileWriterFactory
            ::GenerateWriterFromFileName(fNames.gap_fill_detail()) ;
        if( detail_f == NULL )
            FATAL(" failed to open xxx.gap_fill_detail for write !");
        for( const auto & pair : all_details )
        {
            (*detail_f)<<'>'<<pair.first<<'\n';
            for( const auto & d : pair.second )
            {
                (*detail_f)<<d.now_s<<'\t'
                    <<d.now_e<<'\t'
                    <<d.type;
                if( d.type == 'S' )
                    (*detail_f)<<'\t'<<d.old_s<<'\t'
                        <<d.old_e<<'\t';
                (*detail_f)<<'\n';
            }
        }
        delete detail_f;
    }

    std::map<long , std::string > name_map ;
    void LoadNameMap()
    {
        auto in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName(fNames.name_map());
        if( in == NULL )
            FATAL("failed to open xxx.name_map to read");
        auto readline = [this](const std::string & line )
        {
            long id = -1 ; std::string name ;
            std::istringstream ist(line);
            ist>>id >>name;
            if( id >= 0 )
                name_map[id] = name ;
        };
        BGIQD::FILES::FileReaderFactory::EachLine(*in,readline);
        delete in ;
    }
} config;


int main(int argc , char ** argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string, prefix ,"prefix of files.\n\
                                            Input\n\
                                                    xxx.contig\n\
                                                    xxx.updated_scaff_infos\n\
                                                    xxx.name_map\n\
                                            Output\n\
                                                    xxx.scaff_seqs\n\
                                                    xxx.fill_detail");
    END_PARSE_ARGS;
    config.Init( prefix.to_string() );
    BGIQD::LOG::timer t(config.loger,"TGSSeqGen");

    config.LoadScaffInfos();
    config.LoadContigs();
    config.LoadNameMap();
    config.PrintScaffSeqs();
    config.PrintDetails();
    return 0;
}
