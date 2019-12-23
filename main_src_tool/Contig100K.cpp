#include "biocommon/fasta/fasta.h"
#include "common/files/file_writer.h"
#include "common/files/file_reader.h"
#include "common/args/argsparser.h"
#include "common/error/Error.h"
#include "common/log/log.h"
#include "common/log/logfilter.h"

#include <map>
#include <cassert>

typedef BGIQD::FASTA::Id_Desc_Head AHeader;
typedef BGIQD::FASTA::Fasta<AHeader> AONTRead;
typedef BGIQD::FASTA::FastaReader<AONTRead> AReader ;
typedef BGIQD::SEQ::seq Seq;

std::map<std::string, Seq> reads;
std::map<std::string, std::map<int,Seq> > out_reads;
std::map<std::string, std::map<int,Seq> > discard_reads;

int main(int argc , char ** argv )
{
    ///////////////////////////////////////////////////
    //
    // step 0 . parse args. 
    //
    ///////////////////////////////////////////////////
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string,input_fa ,"input contigs in fasta format file.");
        DEFINE_ARG_REQUIRED(std::string,prefix,"out prefix. will generate xxx.out.fa && xxx.discard.fa.");
        DEFINE_ARG_OPTIONAL(int , chunk_size, "chunk size" , "100000");
    END_PARSE_ARGS
    BGIQD::LOG::logger loger;
    BGIQD::LOG::logfilter::singleton().
        get("Contig100K",BGIQD::LOG::loglevel::INFO, loger);
    int chunk = chunk_size.to_int() ;
    loger<<BGIQD::LOG::lstart()<<" chunk_size is  : "<<chunk<<BGIQD::LOG::lend();

    ///////////////////////////////////////////////////
    //
    // step 1 . load input reads. 
    //
    ///////////////////////////////////////////////////
    auto in = BGIQD::FILES::FileReaderFactory
        ::GenerateReaderFromFileName(input_fa.to_string());
    if ( in == NULL )
        FATAL(" failed to open the input_fa to read ");
    AReader reader ;
    AONTRead tmp ;
    long total_input = 0 ;
    int contig_num = 0 ;
    while( reader.LoadNextFasta(*in ,tmp) )
    {
        reads[tmp.head.Id] = tmp.seq ;
        total_input+= tmp.seq.Len();
        contig_num ++ ;
    }
    delete in ;
    loger<<BGIQD::LOG::lstart()<<" Total load "<<contig_num<<" contigs."<<BGIQD::LOG::lend();
    loger<<BGIQD::LOG::lstart()<<" Total size is "<<total_input<<" bp."<<BGIQD::LOG::lend();
    ///////////////////////////////////////////////////
    //
    // step 2 . split input reads into trunks 
    //
    ///////////////////////////////////////////////////
    for( const auto & pair : reads ) {
        const auto & name = pair.first ;
        const auto & seq = pair.second ;
        int i = 0 ; 
        for( i = 0 ; seq.Len() - i > chunk ; i+=chunk ) {
            Seq tmp ;
            tmp.AddPartSeq(seq.atcgs.substr(i,chunk));
            out_reads[name][i] = tmp ;
        }
        if ( i < seq.Len() -1 ) {
            assert( seq.Len() - i < chunk );
            Seq tmp ;
            tmp.AddPartSeq(seq.atcgs.substr(i,seq.Len() - i));
            discard_reads[name][i] = tmp ;
        }
    }
    ///////////////////////////////////////////////////
    //
    // step 3 .print chunks. 
    //
    ///////////////////////////////////////////////////
    auto out = BGIQD::FILES::FileWriterFactory
        ::GenerateWriterFromFileName(prefix.to_string() + "out.fa");
    if( !out ) 
        FATAL(" failed to open xxx.out.fa to write.");
    for( const auto & pair : out_reads ) {
        const auto & name = pair.first ;
        const auto & seqs = pair.second ;
        for( const auto & pair1 : seqs ) { 
            (*out)<<">"<<name<<"_"<<pair1.first<<'\n';
            (*out)<<pair1.second.Seq(60);
        }
    }
    delete out ;


    auto out1 = BGIQD::FILES::FileWriterFactory
        ::GenerateWriterFromFileName(prefix.to_string() + "discard.fa");
    if( !out1 ) 
        FATAL(" failed to open xxx.discard.fa to write.");
    for( const auto & pair : discard_reads ) {
        const auto & name = pair.first ;
        const auto & seqs = pair.second ;
        for( const auto & pair1 : seqs ) { 
            (*out1)<<">"<<name<<"_"<<pair1.first<<'\n';
            (*out1)<<pair1.second.Seq(60);
        }
    }
    delete out1 ;
    return 0 ;
}
