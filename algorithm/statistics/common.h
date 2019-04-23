#ifndef __ALORITHM_STATISTICS_COMMON_H__
#define __ALORITHM_STATISTICS_COMMON_H__

#include <vector>
#include <math.h>

namespace BGIQD {
    namespace Statistics {

        template<class T, class Collection = std::vector<T> >
            void Average( const Collection & data, T & ret )
            {
                T total = 0 ;
                for( const auto & d : data )
                {
                    total += d ;
                }
                ret = total / T(data.size());
            }

        template <class T, class Collection = std::vector<T> >
            void SD(const Collection & data, const T average , T & ret)
            {
                T sd = 0 ;
                for( const auto & d : data )
                {
                    sd += (d-average)*(d-average) ;
                }
                ret = std::sqrt( sd / T(data.size()));
            }
    }
}

#endif //__ALORITHM_STATISTICS_COMMON_H__
