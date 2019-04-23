#include "common/args/argsparser.h"
#include "common/log/log.h"
#include "common/log/logfilter.h"
#include "common/files/file_writer.h"
#include "common/files/file_reader.h"
#include "common/error/Error.h"

#include "biocommon/fasta/fasta.h"
#include "biocommon/seq/tool_func.h"

#include "stLFR/fileName.h"

#include "stLFR/ScaffInfo.h"

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
        BGIQD::LOG::logfilter::singleton().get("ScaffInfo2Seqs",BGIQD::LOG::loglevel::INFO, loger);
    }

    void LoadScaffInfos()
    {
        auto  in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName(fNames.scaff_infos());
        if( in == NULL )
            FATAL("failed to open xxx.scaff_infos to read");

        scaff_helper.LoadAllScaff(*in);
        delete in ;
    }

    void LoadAllFasta()
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

    void PrintScaffSeqs()
    {
        auto out = BGIQD::FILES::FileWriterFactory
            ::GenerateWriterFromFileName(fNames.scaff_seqs()) ;

        auto get_atcg = [&] ( const BGIQD::stLFR::ContigDetail & detail )
        {
            std::string str = contigs.at(detail.contig_id).seq.atcgs ;
            if( ! detail.orientation )
                str = BGIQD::SEQ::seqCompleteReverse(str);
            if( detail.gap_size > 0 )
            {
                if( detail.extra.find(BGIQD::stLFR::ContigDetail::ONT_FILL) 
                        != detail.extra.end() )
                {
                    str += detail.extra.at(BGIQD::stLFR::ContigDetail::ONT_FILL);
                }
                else
                {
                    int gap_size = detail.gap_size > min_n ?  detail.gap_size : min_n ;
                    str += std::string(gap_size,'N');
                }
            }
            else if ( detail.gap_size < 0 )
            {
                if(  (int)str.length() +  (int)std::abs(detail.gap_size) > 0 )
                    str = str.substr(0,str.length() + detail.gap_size);
                str += std::string(min_c,'N');
            }
            else
            {
                // gap size = 0
                ;
            }

            return str ;
        };

        for( const auto & pair : scaff_helper.all_scaff)
        {
            (*out)<<">scaffold "<<pair.first<<'\n';
            std::string str ;
            for( const auto & i : pair.second.a_scaff) 
            {
                used.insert(i.contig_id);
                str+= get_atcg(i);
            }
            (*out)<<BGIQD::SEQ::blockSeq(str,100);
        }
        for( const auto & pair : contigs )
        {
            if( used.find( pair.first ) == used.end () )
            {
                (*out)<<pair.second.head.Head()<<'\n';
                (*out)<<pair.second.seq.Seq(100);
            }
        }
        delete out ;
    }
    int min_n ;
    int min_c ;
} config;


int main(int argc , char ** argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string, prefix ,"prefix of files. Input xxx.scaff_infos; Output xxx.scaff_seqs");
        DEFINE_ARG_OPTIONAL(int, min_n,"min N size for gap in scaffold if not filled ","11");
        DEFINE_ARG_OPTIONAL(int, min_c,"min N size for contig overlap cut ","11");
    END_PARSE_ARGS;
    config.min_c = min_c.to_int() ;
    config.min_n = min_n.to_int();
    config.Init( prefix.to_string() );
    BGIQD::LOG::timer t(config.loger,"ScaffInfo2Seqs");

    config.LoadScaffInfos();
    config.LoadAllFasta();
    config.PrintScaffSeqs();
    return 0;
}
