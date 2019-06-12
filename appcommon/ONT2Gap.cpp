#include "appcommon/ONT2Gap.h"
#include <algorithm>
#include <cmath>

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
        void SortMatchScoreMore( std::vector<ONT2GapInfo> & data
                ,int max_hang 
                ,float fa
                ,float fb)
        {
            if( data.size() < 2 )
                return ;

            auto score_less =[&] (const ONT2GapInfo & c1 
                    , const ONT2GapInfo & c2 ) -> bool 
            {
                auto score = [&](const BGIQD::PAF::PAF_Item &f,
                    const BGIQD::PAF::PAF_Item &t) -> float 
                {
                    float a1 = float(f.aligned_len)/float(max_hang) * 100 ;
                    float i1 = float(f.match_len)/float(f.aligned_len) * 100 ;
                    float a2 = float(t.aligned_len)/float(max_hang) * 100 ;
                    float i2 = float(t.match_len)/float(t.aligned_len) * 100 ;
                    return fa*log10(a1)
                        +  fb*log10(i1) 
                        +  fa*log10(a2) 
                        +  fb*log10(i2);
                };
                return  score(c1.from,c1.to) < score(c2.from,c2.to);
            };
            std::sort(data.rbegin() , data.rend(), score_less );
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
