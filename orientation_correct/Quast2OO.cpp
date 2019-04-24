#include "contigPairInfo.h"
#include <sstream>
#include <iostream>

struct QuastContig
{
    int r_start ;
    int r_end ;
    int c_start ;
    int c_end ;
    std::string ref ;
    std::string contig_full_name ;

    long contig_num ;
    char contig_2_ref_char ;

    void InitFromString( const std::string & line )
    {
        std::istringstream ist(line);
        ist>>r_start>>r_end>>c_start>>c_end>>ref>>contig_full_name;
    }
    void ParseData()
    {
        std::string cn ;
        for( auto c : contig_full_name )
        {
            if( ! std::isdigit(c) )
                break ;
            cn += c ;
        }
        contig_num = std::stol(cn);
        if( c_start < c_end )
            contig_2_ref_char = '+' ;
        else
            contig_2_ref_char = '-' ;
    }
};

int ContigGapSize( const QuastContig & prev , const QuastContig & next )
{
    return next.r_start - prev.r_end -1 ;
}

int main()
{
    QuastContig prev ;
    prev.ref = "none";

    std::string line ;
    while( ! std::getline( std::cin , line).eof() )
    {
        QuastContig curr ;
        curr.InitFromString( line) ;
        curr.ParseData();
        if( prev.ref == curr.ref )
        {
            BGIQD::stLFR::PairPN tmp ;
            tmp.InitFromRef( prev.contig_num , prev.contig_2_ref_char 
                    , curr.contig_num , curr.contig_2_ref_char , ContigGapSize( prev, curr ) );
            std::cout<<tmp.ToString()<<'\n';
        }
        prev = curr ;
    }
    return 0 ;
}
