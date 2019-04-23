#ifndef __ALGORITHM_MULTIKEYHASH_MULTIKEYHASH_H__
#define __ALGORITHM_MULTIKEYHASH_MULTIKEYHASH_H__

#include <map>
#include <tuple>
#include <cassert>

namespace BGIQD {
    namespace MultiKeyMap {

        //template<class T , class V ,int n = 2 >
        //    struct MultiKeyMap
        //    {
        //        typedef T KeyType ;
        //        typedef V ValueType ;

        //        std::map<
        //    };

        template<class T , class V >
            struct BiKeyHash
            {
                typedef T KeyType ;
                typedef V ValueType ;


                typedef std::pair<KeyType , KeyType> DoubleKey;

                DoubleKey MakeDouleKey( const KeyType & k1 , const KeyType & k2 )
                {
                    if( k1 < k2 )
                    {
                        return std::make_pair(k1,k2);
                    }
                    else
                    {
                        return std::make_pair(k2,k1);
                    }
                }

                bool Contain( const KeyType & k1 , const KeyType & k2)
                {
                    return data.find( MakeDouleKey(k1,k2) ) != data.end() ;
                }

                void Set(const KeyType & k1 , const KeyType & k2, const ValueType & v)
                {
                    data[MakeDouleKey(k1,k2)] = v ;
                }

                ValueType & At(const KeyType & k1 , const KeyType & k2)
                {
                    static ValueType invalid ;
                    auto  itr = data.find( MakeDouleKey(k1,k2) ) ;
                    if( itr == data.end ()) 
                    {
                        assert(0);
                        return invalid;
                    }
                    else
                    {
                        return itr->second ;
                    }
                }

                std::map<std::pair<KeyType , KeyType > , ValueType > data;

            };
    }
}

#endif 
