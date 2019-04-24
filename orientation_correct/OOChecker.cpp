#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cassert>

struct ContigMapInfo
{
    unsigned int contig ;
    char orientation ;
    std::string ref ;
    int start ;
    int end ;
    long index ;

    void InitFromString(const std::string & line )
    {
        std::istringstream ist(line);
        ist >>contig
            >>orientation
            >>ref
            >>start
            >>end
            >>index ;
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
    }

    enum OOType 
    {
        Unknow = 0 ,
        C1_2_C2 = 1 ,
        C1_2_C2p = 2 ,
        C1p_2_C2 = 3 ,
        C1p_2_C2p = 4 
    };

    OOType index() const 
    {
        if (
                 c1_2_c2 >= c1p_2_c2 
                && c1_2_c2 >= c1_2_c2p 
                && c1_2_c2 >= c1p_2_c2p ) 
            return OOType::C1_2_C2;

        if(        c1_2_c2p >= c1_2_c2 
                && c1_2_c2p >= c1p_2_c2 
                && c1_2_c2p >= c1p_2_c2p ) 
            return OOType::C1_2_C2p ;

        if(        c1p_2_c2 >= c1_2_c2 
                && c1p_2_c2 >= c1_2_c2p 
                && c1p_2_c2 >= c1p_2_c2p ) 
            return OOType::C1p_2_C2 ;

        return OOType::C1p_2_C2p;
    }

    std::string  ToString() const 
    {
        std::ostringstream ost ;
        ost<<c1<<'\t'
            <<c2<<'\t'
            <<c1_2_c2<<'\t'
            <<c1_2_c2p<<'\t'
            <<c1p_2_c2<<'\t'
            <<c1p_2_c2p<<'\t'
            <<type<<'\t'
            <<step;

        return ost.str() ;
    }
};

std::map<unsigned int , ContigMapInfo> data;


pairInfo::OOType get_type(const ContigMapInfo & c1 , const ContigMapInfo & c2 )
{
    assert( c1.contig < c2.contig ) ;
    if( c1.index > c2.index )
    {
        if( c1.orientation == '+' && c2.orientation == '+' )
            return pairInfo::OOType::C1p_2_C2p ;
        else if ( c1.orientation == '+' && c2.orientation == '-' )
            return pairInfo::OOType::C1p_2_C2 ;// 2 ;
        else if ( c1.orientation == '-' && c2.orientation == '+' )
            return pairInfo::OOType::C1_2_C2p ;//1 ;
        else if ( c1.orientation == '-' && c2.orientation == '-' )
            return pairInfo::OOType::C1_2_C2  ;
    }
    else
    {
        if( c1.orientation == '+' && c2.orientation == '+' )
            return pairInfo::OOType::C1_2_C2 ;
        else if ( c1.orientation == '+' && c2.orientation == '-' )
            return pairInfo::OOType::C1_2_C2p  ;
        else if ( c1.orientation == '-' && c2.orientation == '+' )
            return pairInfo::OOType::C1p_2_C2 ;// 2 ;
        else if ( c1.orientation == '-' && c2.orientation == '-' )
            return pairInfo::OOType::C1p_2_C2p  ;
    }
    return pairInfo::OOType::Unknow ;
}


std::string  checker( unsigned int c1 , unsigned int c2 , int type , pairInfo & tmp)
{
    auto itr1 = data.find(c1) ;
    auto itr2 = data.find(c2) ;
    if( itr1 == data.end() || itr2 == data.end() )
    {
        tmp.step = 0 ;
        return "Unmatch";
    }
    if( itr1->second.ref != itr2->second.ref )
    {
        tmp.step = 0 ;
        return "Diff_Chromosome";
    }
    if( get_type( itr1->second , itr2->second ) != type ) 
    {
        tmp.step = 0 ;
        return "Wrong_OO";
    }
    else
    {
        tmp.step = std::abs(itr1->second.index - itr2->second.index );
        return "Correct";
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

    std::ifstream in1;
    in1.open(contig_file);
    std::string line ;
    while( ! std::getline(in1,line).eof() )
    {
        ContigMapInfo tmp ;
        tmp.InitFromString(line);
        data[tmp.contig] = tmp ;
    }
    in1.close();

    std::ifstream in2;
    in2.open(filter_file);
    long index = 0 ;
    long total = 0 ;
    const int buffer_size = 10000 ;
    pairInfo buffer[buffer_size];
    while( ! std::getline(in2,line).eof() )
    {
        pairInfo & tmp = buffer[index];
        tmp.InitFromString(line);
        tmp.type = checker(tmp.c1 , tmp.c2, tmp.index(), tmp);
        index ++ ;
        if( index % buffer_size == 0  )
        {
            total += index ;
            for( int i = 0 ; i < index ; i ++ )
            {
                std::cout<<buffer[i].ToString()<<std::endl;
            }
            index = 0 ;
            std::cerr<<" Processing "<<total<<" lines now ... "<<std::endl;
        }
    }

    in2.close();
    total += index ;
    for( int i = 0 ; i < index ; i ++ )
    {
        std::cout<<buffer[i].ToString()<<std::endl;
    }
    index = 0 ;
    std::cerr<<" Processing "<<total<<" lines now ... "<<std::endl;
    std::cerr<<" Done"<<std::endl;
    return 0 ;
}
