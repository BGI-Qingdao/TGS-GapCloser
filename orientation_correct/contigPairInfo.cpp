#include "contigPairInfo.h"
#include <sstream>

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

        std::string PairPN::ToString() const
        {
            std::ostringstream ost ;
            ost<<c1<<'\t'<<c2<<'\t'<<OOToString(type)<<'\t'<<gap_size;
            return ost.str() ;
        }
    }
}
