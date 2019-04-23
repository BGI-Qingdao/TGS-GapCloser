#ifndef __COMMON_MIDDLE_VALID_H__
#define __COMMON_MIDDLE_VALID_H__ 

#include <tuple>
#include <map>
#include <cassert>
#include <vector>
#include <algorithm>

namespace BGIQD {
    namespace MIDDLE_VALID {

        template<class Key>
            std::tuple<long,long> MiddleValid(
                    const std::map<Key,long> & freq
                    ,float fac )
            {
                assert(fac <= 1.0f );
                assert(fac >= 0.0f );
                assert(!freq.empty()) ;
                long total = 0 ;
                std::vector<long> data;
                for( const auto & pair : freq )
                {
                    data.push_back(pair.second);
                    total += pair.second ;
                }
                long target = total * fac ;
                std::sort(data.rbegin() , data.rend());
                long biggest = data[0];
                long smallest = data[0];
                long sum = 0 ;
                for( size_t i = 0 ; i < data.size() ; i ++ )
                {
                    sum += data[i] ;
                    if( sum >= target )
                    {
                        smallest = data[i] ;
                        break ;
                    }
                }
                return std::make_tuple(smallest,biggest);
            }
    }
}

#endif
