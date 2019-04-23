#ifndef __COMMON_STL_MAPHELPER_H__
#define __COMMON_STL_MAPHELPER_H__
#include <map>

namespace BGIQD{
    namespace STL{
        template<class Map>
            struct MapHelper
            {
                typedef typename Map::key_type Key;
                typedef typename Map::mapped_type Value;

                static void Incr(Map & map , const Key & k , const Value & value)
                {
                    auto itr = map.find(k);
                    if( itr == map.end() )
                    {
                        map[k]=value;
                    }
                    else
                        itr->second += value;
                }
                static void UpdateAsBiggest(Map & map , const Key & k , const Value & value)
                {
                    auto itr = map.find(k);
                    if( itr == map.end() )
                    {
                        map[k] =value;
                    }
                    else if( itr->second < value )
                        itr->second = value;
                }
                static void UpdateAsSmallest(Map & map , const Key & k , const Value & value)
                {
                    auto itr = map.find(k);
                    if( itr == map.end() )
                    {
                        map[k] =value;
                    }
                    else if( itr->second > value )
                        itr->second = value;
                }
            };
    }
}



#endif 
