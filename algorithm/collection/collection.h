#ifndef __ALGORITHM_COLLECTION_COLLECTION_H__
#define __ALGORITHM_COLLECTION_COLLECTION_H__ 

#include <map>
#include <cassert>

namespace BGIQD{
    namespace Collection{

        // NOT THREAD SAFE!!!

        template<class T>
            struct Collection
            {
                typedef T Element;

                Collection() {}

                Collection(const Collection & other ) 
                {
                    if( &other != this )
                    {
                        elements.clear();
                        elements = other.elements ;
                    }
                }

                Collection & operator = ( const Collection & other) 
                {
                    if( &other != this )
                    {
                        elements.clear();
                        elements = other.elements ;
                    }
                    return *this;
                }

                void IncreaseElement(const Element & e, int num = 1)
                {
                    assert( num > 0 );
                    auto itr = elements.find(e);
                    if ( itr == elements.end() )
                    {
                        elements[e] = num ;
                    }
                    else
                        itr->second += num ;
                }

                void ReduceElement(const Element & e, int num = 1)
                {
                    assert( num > 0 );
                    auto itr = elements.find(e);
                    if ( itr == elements.end() )
                    {
                        assert(0);
                        return ;
                    }
                    else if ( itr->second < num )
                    {
                        assert(0);
                    }
                    itr->second -= num ;
                    if( itr->second < 1 )
                    {
                        elements.erase(itr);
                    }
                }

                size_t size() const 
                {
                    size_t ret = 0 ;
                    for( const auto & i : elements)
                    {
                        ret += i.second ;
                    }
                    return ret ;
                }

                size_t keysize() const 
                {
                    return elements.size() ;
                }

                static Collection Intersection( const Collection & c1 , const Collection & c2 )
                {
                    Collection ret ;
                    const Collection * small , *big ;
                    if( c1.keysize() < c2.keysize() )
                    {    small = &c1 , big =& c2; }
                    else
                    {    small = &c2 , big =& c1; }
                    for( const auto & i : small->elements )
                    {
                        auto itr = big->elements.find(i.first);
                        if( itr == big->elements.end())
                            continue ;
                        ret.IncreaseElement( i.first , i.second < itr->second ? i.second : itr->second);
                    }
                    return ret ;
                }

                static Collection Union(const Collection & c1 , const Collection & c2 )
                {
                    Collection ret ;
                    ret += c1 ;
                    ret += c2 ;
                    ret -= Intersection(c1,c2);
                    return ret ;
                }

                static float Jaccard(const Collection & c1 , const Collection & c2 )
                {
                    auto it = Intersection(c1,c2) ;
                    Collection ret ;
                    ret += c1 ;
                    ret += c2 ;
                    ret -= it ;
                    return ((float)it.size()) / (float)((ret).size()) ;
                }

                Collection & operator +=( const Collection & other)
                {
                    for( const auto i : other.elements )
                    {
                        IncreaseElement(i.first ,i.second );
                    }
                    return *this;
                }

                Collection & operator -= (const Collection & other )
                {
                    for( const auto i : other.elements )
                    {
                        ReduceElement(i.first ,i.second );
                    }
                    return *this;
                }

                std::map<Element , int > elements;

                typedef typename std::map<Element , int >::const_iterator iterator;

                iterator begin() const
                {
                    return elements.begin() ;
                }

                iterator end()  const 
                {
                    return elements.end() ;
                }

            };
    }
}

#endif //__ALGORITHM_COLLECTION_COLLECTION_H__
