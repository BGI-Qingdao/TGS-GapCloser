#ifndef __STLFR_CONTIGPAIRINFO_H__
#define __STLFR_CONTIGPAIRINFO_H__

#include <cassert>
#include <string>

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

        static inline std::string OOToString( OOType type )
        {
            if( type == OOType::Unknow )
                return "**" ;
            else if ( type == OOType::C1_2_C2 )
                return "++" ;
            else if ( type == OOType::C1p_2_C2 )
                return "-+" ;
            else if ( type == OOType::C1_2_C2p )
                return "+-" ;
            else
                return "--" ;
        }

        struct PairPN
        {
            unsigned int c1 ;
            unsigned int c2 ;
            OOType type ;
            int gap_size ;

            void InitFromRef( unsigned int cc1 , char t1 
                    , unsigned int cc2 , char t2 , int  gap ) ;

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

            std::string ToString() const ;
        };
    }
}

#endif //__STLFR_CONTIGPAIRINFO_H__
