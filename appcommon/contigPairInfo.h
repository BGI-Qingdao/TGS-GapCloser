#ifndef __STLFR_CONTIGPAIRINFO_H__
#define __STLFR_CONTIGPAIRINFO_H__

#include <cassert>

#include "appcommon/ScaffInfo.h"

#include "biocommon/paf/PAF.h"

namespace BGIQD {
    namespace stLFR {

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
            int gap_size ;

            void InitFromRef( unsigned int cc1 , char t1 
                    , unsigned int cc2 , char t2 , int  gap ) ;

            void InitFromScaffInfo( const ContigDetail & prev 
                    ,  const ContigDetail & next )
            {
                InitFromRef(prev.contig_id , ( prev.orientation ? '+' : '-')
                        , next.contig_id ,( next.orientation ? '+' : '-' )
                        , prev.gap_size );
            }

            void InitFromPAF( const BGIQD::PAF::PAF_Item & g1,
                    const BGIQD::PAF::PAF_Item & g2) ;

            bool operator < ( const PairPN & other ) const
            {
                if( c1 != other.c1 )
                    return c1 < other.c1 ;
                if( c2 != other.c2 )
                    return c2 < other.c2 ;
                if(  static_cast<int>(type) != static_cast<int>(other.type) )
                    return  static_cast<int>(type) < static_cast<int>(other.type);
                return gap_size < other.gap_size ;
            }

        };
    }
}

#endif //__STLFR_CONTIGPAIRINFO_H__
