#ifndef __COMMON_FREQ_FREQ_H__
#define __COMMON_FREQ_FREQ_H__
#include <map>
#include <sstream>
namespace BGIQD{
    namespace FREQ{

        template< class Key >
            class Freq
            {
                public:
                    void Touch( const Key k , int num = 1)
                    {
                        if( data.find(k) != data.end() )
                            data[k] += num  ;
                        else
                            data[k] = num ;
                    }

                    std::string ToString() const
                    {
                        std::ostringstream ret ;
                        for( const auto i : data )
                            ret<<i.first<<'\t'<<i.second<<std::endl;
                        return ret.str();
                    }
                    std::map<Key, long> data;

                    long GetFreq(const Key k) const 
                    {
                        if( data.find( k ) == data.end () )
                            return 0 ;
                        else 
                            return data.at(k);
                    }
            };
    }
}

#endif
