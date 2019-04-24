#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <cassert>

#include "ScaffInfo.h"

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
        if ( t1 == '+' && t2 == '+')
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

// ONT provide data
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
                if( c1_2_c2 >= c1p_2_c2 
                && c1_2_c2 >= c1_2_c2p 
                && c1_2_c2 >= c1p_2_c2p ) 
            return OOType::C1_2_C2;

        if(        c1_2_c2p >= c1_2_c2 
                && c1_2_c2p >= c1p_2_c2 
                && c1_2_c2p >= c1p_2_c2p ) 
            return OOType::C1_2_C2p;

        if(        c1p_2_c2 >= c1_2_c2 
                && c1p_2_c2 >= c1_2_c2p 
                && c1p_2_c2 >= c1p_2_c2p ) 
            return OOType::C1p_2_C2;

        return OOType::C1p_2_C2p;
    }

    int GetCount() const 
    {
                if( c1_2_c2 >= c1p_2_c2 
                && c1_2_c2 >= c1_2_c2p 
                && c1_2_c2 >= c1p_2_c2p ) 
            return c1_2_c2 ;

        if(        c1_2_c2p >= c1_2_c2 
                && c1_2_c2p >= c1p_2_c2 
                && c1_2_c2p >= c1p_2_c2p ) 
            return c1_2_c2p ;

        if(        c1p_2_c2 >= c1_2_c2 
                && c1p_2_c2 >= c1_2_c2p 
                && c1p_2_c2 >= c1p_2_c2p ) 
            return c1p_2_c2 ;
        return c1p_2_c2p ;
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

struct ContigOrientationVote
{
    unsigned int contig_id ;
    int b_count ;
    int p_count ;
    bool from_scaff ;
    void Init(unsigned int c, bool pos )
    {
        from_scaff = pos ;
        contig_id = c ;
        if( pos )
        {
            b_count = 1 ;
            p_count = 0 ;
        }
        else
        {
            b_count = 0 ;
            p_count = 1 ;
        }
    }
    bool IsPos() const 
    {
        if ( p_count == b_count ) return from_scaff ;
        return p_count < b_count ;
    }
};

std::map< unsigned int , ContigOrientationVote>  vote_data ;

int main( int argc , char ** argv )
{
    if ( argc != 3 )
    {
        std::cerr<<"arg num must be 2 but is "<<argc-1<<std::endl;
        std::cerr<<argv[0]<<" scaff_info ont_pair"<<std::endl;
        return -1;
    }
    std::string scaff_info(argv[1]);
    std::string ont_pair(argv[2]);

    std::cerr<<" scaff info  file ( "<<scaff_info<<" )"<<std::endl;
    std::cerr<<" ont pair file ( "<<ont_pair<<" )"<<std::endl;

    // cache scaff data first .
    std::ifstream in1;
    in1.open(scaff_info);
    BGIQD::stLFR::ScaffInfoHelper helper ;
    helper.LoadAllScaff(in1);
    in1.close() ;
    helper.FormatAllIndex();
    for( const auto & pair : helper.contig_indexs)
    {
        const auto  & contig_id = pair.first ;
        const auto & contig_detail = helper.GetContig(contig_id);
        vote_data[contig_id].Init(contig_id, contig_detail.orientation);
    }

    std::ifstream in2;
    in2.open(ont_pair);
    std::string line ;
    while( ! std::getline(in2,line).eof() )
    {
        pairInfo tmp ;
        tmp.InitFromString(line);
        auto pn = tmp.GetPN() ;
        if( ! helper.HasContig( pn.c1 ) || ! helper.HasContig( pn.c2 ) )
        {
            continue ;
        }
        const auto & contig_detail_c1 = helper.GetContig(pn.c1);
        const auto & contig_detail_c2 = helper.GetContig(pn.c2);
        if( contig_detail_c1.scaff_id != contig_detail_c2.scaff_id )
        {
            continue ;
        }
        int count = tmp.GetCount() ;
        if( pn.type == OOType::C1_2_C2 )
        {
            if( contig_detail_c1.scaff_index < contig_detail_c2.scaff_index )
            {
                vote_data[pn.c1].b_count += count;
                vote_data[pn.c2].b_count += count ;
            }
            else
            {
                vote_data[pn.c1].p_count += count ;
                vote_data[pn.c2].p_count += count  ;
            }
        }

        if( pn.type == OOType::C1_2_C2p )
        {
            if( contig_detail_c1.scaff_index < contig_detail_c2.scaff_index )
            {
                vote_data[pn.c1].b_count += count ;
                vote_data[pn.c2].p_count += count ;
            }
            else
            {
                vote_data[pn.c1].p_count += count ;
                vote_data[pn.c2].b_count += count ;
            }
        }

        if( pn.type == OOType::C1p_2_C2p )
        {
            if( contig_detail_c1.scaff_index < contig_detail_c2.scaff_index )
            {
                vote_data[pn.c1].p_count += count ;
                vote_data[pn.c2].p_count += count ;
            }
            else
            {
                vote_data[pn.c1].b_count += count ;
                vote_data[pn.c2].b_count += count ;
            }
        }
        if( pn.type == OOType::C1p_2_C2 )
        {
            if( contig_detail_c1.scaff_index < contig_detail_c2.scaff_index )
            {
                vote_data[pn.c1].p_count += count ;
                vote_data[pn.c2].b_count += count ;
            }
            else
            {
                vote_data[pn.c1].b_count += count ;
                vote_data[pn.c2].p_count += count ;
            }
        }
    }
    in2.close();
    int change  = 0 ;
    for( const auto & pair : vote_data )
    {
        if( pair.second.from_scaff != pair.second.IsPos() )
        {
            change ++ ;
            auto & contig_detail = helper.GetContig( pair.first ) ;
            contig_detail.orientation = ! contig_detail.orientation ;
        }
    }
    helper.PrintAllScaff(std::cout);
    std::cerr<<"total contig in scaffold is "<<vote_data.size()<<std::endl;
    std::cerr<<"changed contig in scaffold is "<<change<<std::endl;

    return 0 ;
}

