#ifndef __ALGORITHM_DISTRIBUTION_DISTRIBUTION_H__
#define __ALGORITHM_DISTRIBUTION_DISTRIBUTION_H__

#include <map>
#include <vector>

#include "algorithm/interval/Interval.h"
namespace BGIQD {
    namespace DISTRIBUTION {

        template<class T>
            struct IntervalPercent
            {
                typedef BGIQD::INTERVAL::Interval<T>  Item;
                std::map<Item, float> percents;
                float GetPercent(const T & t) const 
                {
                    for( const auto & pair : percents)
                    {
                        if ( pair.first.IsContain(t) )
                        {
                            return pair.second ;
                        }
                    }
                    return 0.0f ;
                }

                std::string ToString() const
                {
                    std::string ret ;
                    for( const auto & pair : percents)
                    {
                        ret += pair.first.ToString() ;
                        ret += '\t';
                        ret += std::to_string(pair.second);
                        ret += '\n';
                    }
                    return ret ;
                }

                std::vector<Item> ValidKeys() const
                {
                    std::vector<Item> ret ;
                    for( const auto & pair : percents)
                    {
                        if ( pair.second > 0.0f )
                        {
                            ret.push_back(pair.first);
                        }
                    }
                    return ret ;
                }

                IntervalPercent GetSubPercent( const std::vector<Item> & keys )
                {
                    IntervalPercent ret ;
                    std::map<Item,float> tmp;
                    float total = 0;
                    for( const auto & i : keys )
                    {
                        tmp[i] = 0 ;
                        for( const auto & pair : percents )
                        {
                            if(  i == pair.first && pair.second > 0 )
                            {
                                tmp[i] = pair.second ;
                                break ;
                            }
                        }
                        total += tmp[i];
                    }
                    if ( total > 0 )
                    {
                        for( const auto  & pair : tmp )
                        {
                            ret.percents[pair.first] = pair.second / total ;
                        }
                    }
                    else
                    {
                        for( const auto  & pair : tmp )
                        {
                            ret.percents[pair.first] = 0;
                        }
                    }
                    return ret ;
                }

                float SD(const IntervalPercent & other , float & sd ) const 
                {
                    float ret = 0.0f ;
                    sd = 0.0f ;
                    for( const auto & pair : other.percents )
                    {
                        if( percents.find( pair.first ) != percents.end())
                        {
                            float sub =  percents.at(pair.first) - pair.second ;
                            sd += (sub*sub) ;
                        }
                        else
                        {
                            ret += pair.second ;
                        }
                    }
                    sd /= other.percents.size() ;
                    return ret ;
                }
            };

        template<class T>
            struct IntervalDistribution
            {
                typedef BGIQD::INTERVAL::Interval<T>  Item;
                std::map<Item, int> freqs;

                std::string ToString() const
                {
                    std::string ret ;
                    for( const auto & pair : freqs)
                    {
                        ret += pair.first.ToString() ;
                        ret += '\t';
                        ret += std::to_string(pair.second);
                        ret += '\n';
                    }
                    return ret ;
                }

                void Init(const T & bin ,const  T &  min ,const T & max )
                {
                    for( T m = min ; m < max ; m += bin )
                    {
                        freqs[Item(m,m+bin-1)] = 0 ;
                    }
                }

                void Count(const T & t , int count  =1 )
                {
                    for( auto & pair : freqs )
                    {
                        if ( pair.first.IsContain(t) )
                        {
                            pair.second += count ;
                            return ;
                        }
                    }
                }

                IntervalDistribution ValidPart() const 
                {
                    IntervalDistribution ret ;
                    for(const auto & pair : freqs )
                    {
                        if ( pair.second > 0 )
                        {
                            ret.freqs[pair.first] = pair.second ;
                        }
                    }
                    return ret ;
                }

                IntervalPercent<T> CalcPercent() const 
                {
                    IntervalPercent<T> ret ;
                    int total = 0 ;
                    for( const auto & pair : freqs )
                    {
                        total += pair.second ;
                    }

                    for( const auto & pair : freqs )
                    {
                        ret.percents[pair.first] = (float)pair.second  / (float)total;
                    }
                    return ret ;
                }
            };
    }

}

#endif //__ALGORITHM_DISTRIBUTION_DISTRIBUTION_H__
