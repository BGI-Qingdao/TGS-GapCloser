#include "stLFR/contigPairInfo.h"

namespace BGIQD {
    namespace stLFR {

        void PairPN::InitFromRef( unsigned int cc1 , char t1 , unsigned int cc2 , char t2 , int gap )
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
                type = GetCR(type) ;
            }
            gap_size = gap ;
        }

        void PairPN::InitFromPAF( const BGIQD::PAF::PAF_Item & cc1 
                , const BGIQD::PAF::PAF_Item & cc2 )
        {
            BGIQD::PAF::PAF_Item ci1 = cc1;
            BGIQD::PAF::PAF_Item ci2 = cc2;

            c1 = std::stoul(cc1.query_name) ;
            c2 = std::stoul(cc2.query_name) ;

            if( c1 > c2 )
            {
                int swap = c2 ; c2 = c1 ; c1 = swap ;
                ci1 = cc2; ci2 = cc1;
            }

            assert( ci1.target_name  == ci2.target_name);

            ci1 = ci1.Flatten() ;
            ci2 = ci2.Flatten() ;

            if ( ci1.target_start < ci2.target_start && ci1.target_end < ci2.target_end)
            {
                gap_size = ci2.target_start - ci1.target_end - 1 ;
                if ( ci1.query_char == '+' && ci2.query_char== '+' )
                {
                    type = OOType::C1_2_C2 ;
                }
                else  if ( ci1.query_char == '+' && ci2.query_char == '-' )
                {
                    type = OOType::C1_2_C2p ;
                }
                else  if ( ci1.query_char == '-' && ci2.query_char == '-' )
                {
                    type = OOType::C1p_2_C2p ;
                }
                else if ( ci1.query_char == '-' && ci2.query_char == '+' )
                {
                    type = OOType::C1p_2_C2 ;
                }
            }
            else if ( ci1.target_start > ci2.target_start && ci1.target_end > ci2.target_end)
            {
                gap_size = ci1.target_start -  ci2.target_end - 1 ;
                if ( ci1.query_char == '+' && ci2.query_char == '+' )
                {
                    type = OOType::C1p_2_C2p ;
                    //link_data[c1][c2].c1p_c2p ++ ;
                }
                else    if ( ci1.query_char == '+' && ci2.query_char == '-' )
                {
                    type = OOType::C1p_2_C2 ;
                    //link_data[c1][c2].c1p_c2 ++ ;
                }
                else if ( ci1.query_char == '-' && ci2.query_char == '-' )
                {
                    //link_data[c1][c2].c1_c2 ++ ;
                    type = OOType::C1_2_C2 ;
                }
                else if ( ci1.query_char == '-' && ci2.query_char == '+' )
                {
                    type = OOType::C1_2_C2p ;
                    //link_data[c1][c2].c1_c2p ++ ;
                }
            }
            else
            {
                type = OOType::Unknow ;
                gap_size = 0 ;
            }
        }
    }
}
