#ifndef __ALGORITHM_INCR_ARRAY_INCR_ARRAY_H__
#define __ALGORITHM_INCR_ARRAY_INCR_ARRAY_H__

#include <cstddef>
#include <cassert>

#include <vector>
#include <iterator>
//
// Incr Array .
//
// A substitute of vector which has below feather
//
// good way
//  * DO NOT copy old element while expend size.
//  * element pointer ALWAYS valid.
// bad way
//  * new element can ONLY append at the end of array.
//  * DO NOT suppert earse element.
//  * DO NOT suppert copy-construct and assign-construct.
//      * can support but why need this ?
//

namespace BGIQD {
    namespace INCRARRAY {

        template<class T>
            struct iterator ;

        template<class T>
            struct const_iterator ;

        template<class T>
            struct  IncrArray
            {
                typedef T Element;

                typedef struct iterator<IncrArray> iterator;
                typedef struct const_iterator<IncrArray> const_iterator;

                virtual ~IncrArray()
                {
                    deep_clean();
                }

                IncrArray() 
                {
                    m_block_size = 0 ;
                    m_curr  = 0 ;
                    m_capacity = 0 ;
                }

                void Init( size_t block_size) 
                {
                    assert(m_capacity == 0);
                    m_block_size = block_size ;
                    m_curr = 0 ;
                    m_headers.push_back(new Element[m_block_size]);
                    m_capacity = m_block_size;
                }

                IncrArray(size_t block_size)
                {
                    Init(block_size);
                }

                IncrArray( const IncrArray & );

                IncrArray & operator = ( const IncrArray & ) ;

                void push_back(const Element & e)
                {
                    assert( m_curr <= m_capacity );
                    if ( m_curr ==  m_capacity )
                    {
                        m_headers.push_back(new Element[m_block_size]);
                        m_capacity += m_block_size ;
                    }
                    assert( m_curr < m_capacity );
                    operator[](m_curr++) = e ;
                }

                Element & at(size_t i ) 
                {
                    assert( i < m_curr );
                    size_t block_num = i / m_block_size ;
                    size_t block_i= i % m_block_size ;
                    if( block_num >= m_headers.size() )
                    {
                        assert(0);
                    }
                    return m_headers[block_num][block_i];
                }

                const Element & at(size_t i ) const 
                {
                    assert( i < m_curr );
                    size_t block_num = i / m_block_size ;
                    size_t block_i= i % m_block_size ;
                    if( block_num >= m_headers.size() )
                    {
                        assert(0);
                    }
                    return m_headers[block_num][block_i];
                }

                Element & operator[](size_t i )
                {
                    assert( i < m_curr );
                    size_t block_num = i / m_block_size ;
                    size_t block_i= i % m_block_size ;
                    if( block_num >= m_headers.size() )
                    {
                        assert(0);
                    }
                    return m_headers[block_num][block_i];
                }

                iterator begin() 
                {
                    iterator ret(*this,0);
                    return ret ;
                }

                iterator end() 
                {
                    iterator ret(*this,m_curr);
                    return ret ;
                }

                const_iterator begin()  const 
                {
                    const_iterator ret(*this,0);
                    return ret ;
                }

                const_iterator end() const
                {
                    const_iterator ret(*this,m_curr);
                    return ret ;
                }
                bool empty() const { return m_curr == 0 ;}

                void clear() { m_curr = 0 ; }

                void deep_clean()
                {
                    for( auto  & i : m_headers )
                    {
                        if( i != NULL )
                        {
                            delete [] i ;
                            i = NULL ;
                        }
                    }
                    m_headers.clear();
                    m_curr = 0 ;
                    m_capacity = 0 ;
                    m_block_size = 0 ;
                }

                size_t size() const { return m_curr ; }

                size_t capacity() const { return m_capacity; }

                void resize( size_t i )
                {
                    if( i <= m_capacity )
                        return ;
                    do {
                        m_headers.push_back(new Element[m_block_size]);
                        m_capacity += m_block_size ;
                    }while( m_capacity < i );
                }

                void init_n_element(size_t i )
                {
                    assert( m_curr == 0 );
                    resize( i) ;
                    m_curr = i ;
                }

                protected:
                size_t m_block_size ;
                size_t m_curr ;
                size_t m_capacity;
                std::vector<Element * > m_headers;
            };


        template<class T>
            struct non_const_traits
            {
                typedef T Base ;
                typedef T * BasePointer;
                typedef typename T::Element Element;
                typedef typename T::Element & reference;
                typedef typename T::Element * pointer;
            };

        template<class T>
            struct const_traits
            {
                typedef const T Base ;
                typedef T const * BasePointer;
                typedef typename T::Element const Element;
                typedef typename T::Element const & reference;
                typedef typename T::Element const * pointer;
            };

        template<class T>
            struct iterator_base : public std::iterator<typename T::Element
                              , size_t 
                              , typename T::pointer
                              , typename T::reference
                              , std::random_access_iterator_tag>
        {
            typedef typename T::Base Base;
            typedef typename T::BasePointer  BasePointer;

            typedef typename T::pointer pointer;

            typedef typename T::reference reference;

            iterator_base( Base & b , size_t i ) : base(&b) , curr(i) {}

            iterator_base( const iterator_base & o ) : base(o.base) , curr(o.curr) {}

            iterator_base & operator = ( const iterator_base & o )
            {
                base = o.base ;
                curr = o.curr ;
                return *this ;
            }

            virtual ~iterator_base() {} ;

            bool operator <= ( const iterator_base & o )const { 
                assert ( base == o.base );
                return curr <= o.curr ; 
            }

            bool operator >= ( const iterator_base & o ) const { 
                assert ( base == o.base );
                return curr >= o.curr ; 
            }

            bool operator != ( const iterator_base & o ) const {
                assert ( base == o.base );
                return curr != o.curr ; 
            }
            bool operator < ( const iterator_base & o )const { 
                assert ( base == o.base );
                return curr < o.curr ; 
            }

            bool operator > ( const iterator_base & o ) const { 
                assert ( base == o.base );
                return curr > o.curr ; 
            }

            bool operator == ( const iterator_base & o ) const {
                assert ( base == o.base );
                return curr == o.curr ; 
            }

            reference operator*() 
            {
                return base->at(curr);
            }

            pointer operator->()
            {
                return &(base->at(curr));
            }

            iterator_base & operator++ (int i)
            {
                curr ++ ;
                return *this;
            }

            iterator_base & operator-- (int i)
            {
                curr -- ;
                return *this;
            }
            iterator_base & operator++ ()
            {
                curr ++ ;
                return *this;
            }

            iterator_base & operator-- ()
            {
                curr -- ;
                return *this;
            }

            iterator_base & operator += ( size_t i)
            {
                curr += i ;
                return *this ;
            }

            iterator_base & operator -= ( size_t i)
            {
                curr += i ;
                return *this ;
            }

            iterator_base  operator + ( size_t i) const 
            {
                iterator_base ret(*this);
                ret.curr += i ;
                return ret;
            }

            iterator_base  operator - ( size_t i) const 
            {
                iterator_base ret(*this);
                ret.curr -= i ;
                return ret;
            }

            protected:
            BasePointer base;
            size_t curr ;
        };

        template<class T>
        struct iterator: public iterator_base<non_const_traits<T> >{
            typedef iterator_base<non_const_traits<T> > F;
            iterator(T & b , size_t i ) : F(b,i) {}

            iterator( const iterator& o ) : F(o) {}

            iterator& operator = ( const iterator& o )
            {
                *this = F(o);
            }
        };

        template<class T>
        struct const_iterator: public iterator_base<const_traits<T> >{
            typedef iterator_base<const_traits<T> > F;

            const_iterator(const T & b , size_t i ) : F(b,i) {}

            const_iterator( const const_iterator& o ) : F(o) {}

            const_iterator& operator = ( const const_iterator& o )
            {
                *this = F(o);
            }
        };

    } // INCRARRAY
} // BGIQD

#endif //__ALGORITHM_INCR_ARRAY_INCR_ARRAY_H__
