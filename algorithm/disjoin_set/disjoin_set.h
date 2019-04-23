#ifndef __ALGORITHM_DISJOIN_SET_DISJOIN_SET_H__
#define __ALGORITHM_DISJOIN_SET_DISJOIN_SET_H__

#include <map>
#include <vector>
#include <cassert>

namespace BGIQD{
namespace Algorithm
{

    template< class Key  >
        class DisJoin_Set{
            struct Node ;
            typedef std::map<Key , Node * > nodeHash;
                struct Node
                {
                    Key key;
                    Node * parent;
                    unsigned depth;
                };
            public :
                void AddConnect( const Key a , const Key b )
                {
                    auto aptr = GetRepresentation(a);
                    auto bptr = GetRepresentation(b);
                    if( aptr->key != bptr->key )
                    {
                        JoinNode(aptr , bptr);
                    }
                }

                Key GetGroup( const Key a )
                {
                    return GetRepresentation(a)->key;
                }

                ~DisJoin_Set() {
                    for( auto & i : m_nodeHash )
                    {
                        delete i.second ;
                    }
                    m_nodeHash.clear();
                    for ( auto & i : m_parentVector )
                        delete i;
                    m_parentVector.clear();
                }
            private:

                void JoinNode(Node * a , Node * b)
                {
                    if( a->depth < b->depth ){
                        a->parent = b ;
                    }else if ( b->depth < a->depth ){
                        b->parent = a ;
                    }else {
                        Node * np = new Node();
                        np->key = a->key ;
                        np->depth = a->depth +1 ;
                        np->parent = NULL ;
                        a->parent = np ;
                        b->parent = np ;
                        m_parentVector.push_back(np);
                    }
                }

                Node * GetRepresentation(Key a)
                {
                    auto itr = m_nodeHash.find(a) ;
                    if( itr == m_nodeHash.end() )
                    {
                        Node * aptr  = new Node();
                        aptr->key = a;
                        aptr->parent = NULL; 
                        aptr->depth = 0 ;
                        m_nodeHash[a] = aptr;
                        return aptr;
                    }
                    return get_representation(itr->second);
                }

                Node * get_representation( Node * a )
                {
                    if ( a == NULL ) assert(0) ;
                    if ( a->parent == NULL ) 
                        return a ;
                    Node * ret = get_representation( a->parent );
                    if( a->parent != ret ) a->parent = ret ;
                    return ret ;
                }
                nodeHash m_nodeHash;
                std::vector<Node*> m_parentVector;
        };
} // namespace Algorithm
} // namespace BGIQD
#endif //__ALGORITHM_DISJOIN_SET_DISJOIN_SET_H__
