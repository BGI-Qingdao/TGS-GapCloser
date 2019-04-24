#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <cassert>

enum OOType 
{
    Unknow = 0 ,
    C1_2_C2 = 1 ,
    C1_2_C2p = 2 ,
    C1p_2_C2 = 3 ,
    C1p_2_C2p = 4 
};

static inline OOType GetCR( OOType type )
{
    if(  type == C1_2_C2 )
    {
        return C1p_2_C2p ;
    }
    if( type == C1p_2_C2p )
    {
        return C1_2_C2 ;
    }
    return type ; 
    //return static_cast<OOType>( 5- static_cast<int>(type) );
}

struct PairPN
{
    unsigned int c1 ;
    unsigned int c2 ;
    OOType type ;

    void InitFromRef( unsigned int cc1 , char t1 , unsigned int cc2 , char t2 )
    {
        assert( cc1 != cc2 );
        c1 = cc1 ;
        c2 = cc2 ;
        if ( t1 == '+' && t2 == '-')
            type = OOType::C1_2_C2 ;
        else if ( t1 == '+' && t2 == '-' )
            type = OOType::C1_2_C2p ;
        else if ( t1 == '-' && t2 == '-' )
            type = OOType::C1p_2_C2p ;
        else if ( t1 == '-' && t2 == '+' )
            type = OOType::C1p_2_C2 ;

        if ( c1 > c2 )
        {
            unsigned int tmp = c1 ;
            c1 = c2 ;
            c2 = tmp ;

            type = GetCR(type) ;// static_cast<OOType>( 5- static_cast<int>(type) );
        }
    }

    bool operator < ( const PairPN & other ) const
    {
        if( c1 != other.c1 )
            return c1 < other.c1 ;
        if( c2 != other.c2 )
            return c2 < other.c2 ;
        return static_cast<int>(type) < static_cast<int>(other.type);
    }
};


struct TriConnInfo
{
    unsigned int c1 ;
    unsigned int c2 ;
    unsigned int c3 ;

    char t1 ;
    char t2 ;
    char t3 ;

    bool Valid() const 
    {
        return c1 != c2 && c2 != c3 && c1 != c3 ;
    }

    PairPN GetP1() const 
    {
        PairPN tmp ;
        tmp.InitFromRef( c1 , t1 ,c2, t2 );
        return tmp;
    }

    PairPN GetP2() const 
    {
        PairPN tmp ;
        tmp.InitFromRef( c2 , t2 ,c3, t3 );
        return tmp;
    }

    void InitFromString( const std::string & line )
    {
        std::istringstream ist(line);
        ist>>c1>>c2>>c3>>t1>>t2>>t3 ;
    }
};

struct pairInfo
{
    unsigned int c1 ;
    unsigned int c2 ;
    int c1_2_c2;
    int c1_2_c2p ;
    int c1p_2_c2;
    int c1p_2_c2p ;
    int step ;
    std::string type ;

    void InitFromString(const std::string & line )
    {
        std::istringstream ist(line);
        ist >>c1
            >>c2
            >>c1_2_c2
            >>c1_2_c2p
            >>c1p_2_c2
            >>c1p_2_c2p;
        assert( c1 < c2 );
    }

    OOType index() const 
    {
        if(        c1_2_c2 >= c1_2_c2 
                && c1_2_c2 >= c1p_2_c2 
                && c1_2_c2 >= c1_2_c2p 
                && c1_2_c2 >= c1p_2_c2p ) 
            return OOType::C1_2_C2;

        if(        c1_2_c2p >= c1_2_c2 
                && c1_2_c2p >= c1p_2_c2 
                && c1_2_c2p >= c1_2_c2p 
                && c1_2_c2p >= c1p_2_c2p ) 
            return OOType::C1_2_C2p;

        if(        c1p_2_c2 >= c1_2_c2 
                && c1p_2_c2 >= c1p_2_c2 
                && c1p_2_c2 >= c1_2_c2p 
                && c1p_2_c2 >= c1p_2_c2p ) 
            return OOType::C1p_2_C2;

        return OOType::C1p_2_C2p;
    }

    PairPN GetPN() const 
    {
        PairPN tmp ;
        tmp.c1 = c1 ;
        tmp.c2 = c2 ;
        tmp.type = index();
        return tmp ;
    }
};

std::set<PairPN> data;
std::set<unsigned int> contigs_cache;

void  checker( const TriConnInfo & info , const std::string & line )
{
    if( ! info.Valid() )
    {
        std::cout<<line<<'\t'<<"misassembled"<<'\n';
        return ;
    }
    if (    contigs_cache.find( info.c1 ) == contigs_cache.end() 
        ||  contigs_cache.find( info.c2 ) == contigs_cache.end()
        ||  contigs_cache.find( info.c3 ) == contigs_cache.end()
            )
    {
        std::cout<<line<<'\t'<<"no_reads"<<'\n';
        return ;
    }

    auto itr1 = data.find(info.GetP1()) ;
    auto itr2 = data.find(info.GetP2()) ;
    if( itr1 == data.end() && itr2 == data.end() )
    {
        std::cout<<line<<'\t'<<"123"<<'\n';
        return ;
    }
    else if ( itr1 == data.end() )
    {
        std::cout<<line<<'\t'<<"12"<<'\n';
        return ;
    }
    else if ( itr2 == data.end() )
    {
        std::cout<<line<<'\t'<<"23"<<'\n';
        return ;
    }
}


int main( int argc , char ** argv )
{
    if ( argc != 3 ) 
    {
        std::cerr<<"arg num must be 2 but is "<<argc-1<<std::endl;
        std::cerr<<argv[0]<<" pair_info_file contig_map_file"<<std::endl;
        return -1;
    }
    std::string filter_file(argv[1]);
    std::string contig_file(argv[2]);

    std::cerr<<" pair filter file ( "<<filter_file<<" )"<<std::endl;
    std::cerr<<" contig map  file ( "<<contig_file<<" )"<<std::endl;

    // cache ONT data first .
    std::ifstream in1;
    in1.open(filter_file);
    std::string line ;
    while( ! std::getline(in1,line).eof() )
    {
        pairInfo tmp ;
        tmp.InitFromString(line);
        data.insert(tmp.GetPN()) ;
        contigs_cache.insert(tmp.c1);
        contigs_cache.insert(tmp.c2);
    }
    in1.close();

    // filter all scaffold info.

    std::ifstream in2;
    in2.open(contig_file);
    long index = 0 ;
    while( ! std::getline(in2,line).eof() )
    {
        index ++ ;
        TriConnInfo tmp ;
        tmp.InitFromString(line);
        checker(tmp,line);
        if( index % 10000 == 0 )
        {
            std::cerr<<"processing "<<index<<"... "<<std::endl;
        }
    }
    in2.close();
    std::cerr<<"processing "<<index<<" and finish now"<<std::endl;
    return 0 ;
}
