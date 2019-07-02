#include "common/args/argsparser.h"
#include "biocommon/fasta/fasta.h"

#include <map>
#include <iostream>
#include <fstream>

int main(int argc , char ** argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string, before,"ont.fa before polish");
        DEFINE_ARG_REQUIRED(std::string, after ,"ont.fa after  polish");
    END_PARSE_ARGS;
    std::map<std::string, std::string> before_seqs;
    std::map<std::string, std::string> after_seqs;

    typedef BGIQD::FASTA::Id_Desc_Head Header;
    typedef BGIQD::FASTA::Fasta<Header> Fa;
    typedef BGIQD::FASTA::FastaReader<Fa> Reader ;
    Reader reader ;
    Fa tmp ;
    // Load before
    std::ifstream ifb;
    ifb.open(before.to_string());
    while( reader.LoadNextFasta(ifb ,tmp ) )
    {
        before_seqs[tmp.head.Id] = tmp.seq.atcgs ;
    }
    // Load after 
    std::ifstream ifa;
    ifa.open(after.to_string());
    while( reader.LoadNextFasta(ifa ,tmp ) )
    {
        after_seqs[tmp.head.Id] = tmp.seq.atcgs ;
    }
    int not_found = 0 ;
    int not_change = 0 ;
    int changed = 0 ;
    int total = 0 ;
    // compare
    for( const auto & i : before_seqs)
    {
        std::string after_name = i.first + "_pilon";
        if( after_seqs.find( after_name ) == after_seqs.end() )
            not_found ++ ;
        else if ( after_seqs[after_name] == i.second )
            not_change ++ ;
        else 
            changed ++ ;
        total ++ ;
    }
    std::cout<<"Not_Change\t"<<not_change<<'\n';
    std::cout<<"Changed\t"<<changed<<'\n';
    std::cout<<"Not_Found\t"<<not_found<<'\n';
    std::cout<<"Total\t"<<total<<'\n';
    return 0 ;
}
