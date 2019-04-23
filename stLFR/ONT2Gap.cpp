#include "stLFR/ONT2Gap.h"
#include <algorithm>
namespace BGIQD {
    namespace ONT {


        void SortMore( ONT2GapInfos & data) 
        {
            if( data.size() < 2 )
                return ;
            auto gap_more =[] (const ONT2GapInfo & c1 
                    , const ONT2GapInfo & c2 ) -> bool 
            {
                if( c1.pair_info.gap_size > c2.pair_info.gap_size )
                    return true ;
                else 
                    return false ;
            };
            std::sort(data.begin() , data.end(), gap_more );
        }


        void SortLess( ONT2GapInfos & data)
        {
            if( data.size() < 2 )
                return ;
            auto gap_less =[] (const ONT2GapInfo & c1 
                    , const ONT2GapInfo & c2 ) -> bool 
            {
                if( c1.pair_info.gap_size < c2.pair_info.gap_size )
                    return true ;
                else 
                    return false ;
            };
            std::sort(data.begin() , data.end(), gap_less );
        }

        void SortMedian( ONT2GapInfos & data )
        {
            if( data.size() < 3 )
                return ;
            std::vector<int> gap_sizes;
            for( const auto & i : data )
            {
                gap_sizes.push_back(i.pair_info.gap_size);
            }
            std::sort(gap_sizes.begin() , gap_sizes.end());
            int median = gap_sizes.at(gap_sizes.size()/2);

            auto gap_less =[median] (const ONT2GapInfo & c1 
                    , const ONT2GapInfo & c2 ) -> bool 
            {
                if( std::abs(c1.pair_info.gap_size - median) < std::abs(c2.pair_info.gap_size -median ))
                    return true ;
                else 
                    return false ;
            };
            std::sort(data.begin() , data.end(), gap_less );
        }

        void SortModifyLess( std::vector<ONT2GapInfo> & data )
        {
            if( data.size() < 2 )
                return ;
            auto gap_less =[] (const ONT2GapInfo & c1 
                    , const ONT2GapInfo & c2 ) -> bool 
            {
                if( std::abs(c1.pair_info.gap_size) < std::abs(c2.pair_info.gap_size) )
                    return true ;
                else  if ( std::abs(c1.pair_info.gap_size) == std::abs(c2.pair_info.gap_size) )
                {
                    if ( c1.pair_info.gap_size < c2.pair_info.gap_size )
                        return true ;
                    else 
                        return false ;
                }
                return false ;
            };
            std::sort(data.begin() , data.end(), gap_less );
        }
        /*
        void SortNegModifyLess( std::vector<ONT2GapInfo> & data )
        {
            if( data.size() < 2 )
                return ;
            std::vector<ONT2GapInfo> negs;
            std::vector<ONT2GapInfo> poss;
            for( const auto & i : data )
            {
                if( i.pair_info.gap_size < 0 )
                    negs.push_back(i);
                else
                    poss.push_back(i);
            }
            if( negs.size() > 1 )
                std::sort(negs.rbegin() , negs.rend());
            if( poss.size() > 1 )
                std::sort(poss.begin() , poss.end());
            data.clear();
            for(const auto & i : negs )
                data.push_back(i);
            for(const auto & i : poss)
                data.push_back(i);
        }*/
    }
}
