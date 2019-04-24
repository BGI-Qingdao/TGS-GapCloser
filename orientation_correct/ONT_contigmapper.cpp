#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <stdlib.h>

struct MapInfo
{
    std::string contig_name ;
    int contig_len ;
    int contig_start ;
    int contig_end ;
    char contig_orientation ;
    std::string read_name ;
    int read_len ;
    int read_start ;
    int read_end ;
    int match_len ;
    int alian_len ;
    int MQ ;

    void InitFromString(const std::string & line )
    {
        std::istringstream ist(line);
        ist>>contig_name
            >>contig_len
            >>contig_start
            >>contig_end
            >>contig_orientation
            >>read_name
            >>read_len
            >>read_start
            >>read_end
            >>match_len
            >>alian_len
            >>MQ ;
    }
} ;

//                 contig_name
typedef std::map< std::string  , std::vector<MapInfo> >  AReadMapInfo;;
//                 read_name
typedef std::map< std::string , AReadMapInfo > DataContainer;

DataContainer data;

struct LinkInfo
{
    int c1_c2;
    int c1p_c2;
    int c1_c2p;
    int c1p_c2p;
    int other ;

    LinkInfo() 
        : c1_c2(0)
        , c1p_c2(0) 
        , c1_c2p(0)
        , c1p_c2p(0) {}

    LinkInfo( const LinkInfo & other )
        : c1_c2(other.c1_c2)
        , c1p_c2(other.c1p_c2) 
        , c1_c2p(other.c1_c2p)
        , c1p_c2p(other.c1p_c2p) {}


    LinkInfo & operator = ( const LinkInfo & other )
    {
        if( this != &other )
        {
            c1_c2=other.c1_c2;
            c1p_c2=other.c1p_c2; 
            c1_c2p=other.c1_c2p;
            c1p_c2p=other.c1p_c2p; 
        }
        return *this ;
    }
} ;

typedef std::map<int , std::map<int , LinkInfo > > L1;
typedef std::map<int , LinkInfo > L2;
//       c1              c2
std::map<int , std::map<int , LinkInfo > > link_data ;

void update_link( const MapInfo & info1 , const MapInfo & info2)
{
    MapInfo ci1 = info1;
    MapInfo ci2 = info2;
    int c1 = atoi(info1.contig_name.c_str()) ;
    int c2 = atoi( info2.contig_name.c_str() ) ;
    if( c1 > c2 )
    {
        int swap = c2 ; c2 = c1 ; c1 = swap ;
        ci1 = info2 ; ci2 = info1 ;
    }
    assert( ci1.read_name == ci2.read_name );
    if ( ci1.read_start < ci2.read_start && ci1.read_end < ci2.read_end )
    {
        if ( ci1.contig_orientation == '+' && ci2.contig_orientation == '+' )
        {
            link_data[c1][c2].c1_c2 ++ ;
        }
        else 
        if ( ci1.contig_orientation == '+' && ci2.contig_orientation == '-' )
        {
            link_data[c1][c2].c1_c2p ++ ;
        }
        else 
        if ( ci1.contig_orientation == '-' && ci2.contig_orientation == '-' )
        {
            link_data[c1][c2].c1p_c2p ++ ;
        }
        else
        if ( ci1.contig_orientation == '-' && ci2.contig_orientation == '+' )
        {
            link_data[c1][c2].c1p_c2 ++ ;
        }
    }
    else if ( ci1.read_start > ci2.read_start && ci1.read_end > ci2.read_end )
    {
        if ( ci1.contig_orientation == '+' && ci2.contig_orientation == '+' )
        {
            //link_data[c1][c2].c1_c2 ++ ;
            link_data[c1][c2].c1p_c2p ++ ;
        }
        else 
        if ( ci1.contig_orientation == '+' && ci2.contig_orientation == '-' )
        {
            //link_data[c1][c2].c1_c2p ++ ;
            link_data[c1][c2].c1p_c2 ++ ;
        }
        else 
        if ( ci1.contig_orientation == '-' && ci2.contig_orientation == '-' )
        {
            //link_data[c1][c2].c1p_c2p ++ ;
            link_data[c1][c2].c1_c2 ++ ;
        }
        else
        if ( ci1.contig_orientation == '-' && ci2.contig_orientation == '+' )
        {
            //link_data[c1][c2].c1p_c2 ++ ;
            link_data[c1][c2].c1_c2p ++ ;
        }
    }
}

int  main(int argc , char **argv)
{
    if( argc > 1 )
    {
        std::cerr<<" Usage :"<<argv[0]
                 <<"  < you_contig_map_ont.paf >result.txt "
                 <<std::endl;
        std::cerr<<" exit ... !!! "<<std::endl;
        return 1 ;
    }
    std::string line ;
    MapInfo tmp ;
    std::cerr<<" Load data start ... "<<std::endl;
    while( ! std::getline(std::cin,line).eof() )
    {
        tmp.InitFromString(line);
        data[ tmp.read_name][tmp.contig_name].push_back(tmp) ;
    }
    std::cerr<<" Load data end ... "<<std::endl;
    std::cerr<<" Calc link start ... "<<std::endl;
    for( DataContainer::iterator i = data.begin() ; i != data.end() ; i++ )
    {
         AReadMapInfo  & a_read_info = i->second ;
         for( AReadMapInfo::iterator j = a_read_info.begin() ; j != a_read_info.end() ; j++ )
         {
              AReadMapInfo::iterator jt = j ;
             for( AReadMapInfo::iterator jj = ++jt ;  jj != a_read_info.end() ; jj++ )
             {
                const std::vector<MapInfo> read1_info = j->second ;
                const std::vector<MapInfo> read2_info = jj->second ;
                for( int r1 = 0 ; r1 < read1_info.size() ; r1 ++ )
                {
                    for( int r2 = 0 ; r2 < read2_info.size() ; r2 ++ )
                    {
                        update_link(read1_info[r1] , read2_info[r2]);
                    }
                }
             }
         }
    }
    std::cerr<<" Calc link end ... "<<std::endl;
    std::cerr<<" Print data start ... "<<std::endl;
    for (L1::iterator i = link_data.begin() ; i != link_data.end() ; i++ )
    {
        L2 & l2 = i->second ;
        for( L2::iterator j = l2.begin() ; j != l2.end() ; j++ )
        {
            const LinkInfo & d = j->second ;
            std::cout<<i->first<<'\t'
                     <<j->first<<'\t'
                     <<d.c1_c2<<'\t'
                     <<d.c1_c2p<<'\t'
                     <<d.c1p_c2<<'\t'
                     <<d.c1p_c2p<<'\n' 
                     ;
        }
    }
    std::cerr<<" Print data end ... "<<std::endl;
}

