#ifndef __ALGORITHM_GRAPH_GRAPH_H__
#define __ALGORITHM_GRAPH_GRAPH_H__ 

#include <iterator>
#include <map>
#include <cassert>

namespace BGIQD {
    namespace GRAPH {

        template<class NodeId ,  class EdgeId >
            struct GraphEdgeBase
            {
                typedef NodeId EdgeNodeId;
                typedef EdgeId EdgeEdgeId;

                EdgeId      id ;
                EdgeId      next;

                NodeId      from ;
                NodeId      to ;

                static const EdgeId invalid = -1 ;
            } ;



        template<class NodeId ,  class EdgeId >
            struct GraphNodeBase
            {
                typedef NodeId NodeNodeId;
                typedef EdgeId NodeEdgeId;

                NodeId                          id ;
                EdgeId                          edge_id;
            };


        // Must be implement before use
        template<class BaseGraph 
            , class NodeId 
            , class EdgeId 
            , class Node1 = GraphNodeBase<NodeId,EdgeId> 
            , class Edge1 = GraphEdgeBase<NodeId,EdgeId> 
            >
            struct GraphAccessBase
            {
                typedef NodeId                       GraphNodeId ;
                typedef EdgeId                       GraphEdgeId ;

                typedef Node1                        Node;
                typedef Edge1                        Edge;

                Node & AccessNode(const NodeId &) { assert(0) ; }
                Edge & AccessEdge(const EdgeId & , const GraphNodeId &){ assert(0); }

                BaseGraph * base ;

                std::map<GraphNodeId , Node > nodes ;
                std::map<GraphEdgeId , Edge > edges ;
                std::map<GraphNodeId , Edge > edges_ends;

                template<class Value>
                void GetAttr(const Node & , const Edge & ,
                        const std::string & , Value & ) {assert(0);} 

            };

        template<class GraphAccess>
            struct EdgeIterator : public std::iterator<std::forward_iterator_tag,int>
        {
            public:

                typedef typename GraphAccess::GraphEdgeId Id;
                typedef typename GraphAccess::Edge        Edge;

                EdgeIterator() : curr(NULL) ,accessor( NULL ) { }

                EdgeIterator(const Edge & e , GraphAccess & acc )
                {
                    node_id = e.from;
                    if( e.id != Edge::invalid )
                        curr = &e;
                    else
                        curr = NULL ;
                    accessor = &acc ;
                }

                EdgeIterator( const EdgeIterator & ei )
                {
                    node_id = ei.node_id;
                    curr = ei.curr ;
                    accessor = ei.accessor ;
                }

                EdgeIterator & operator = ( const EdgeIterator & ei )
                {
                    if( &ei != this )
                    {
                        node_id = ei.node_id ;
                        curr = ei.curr ;
                        accessor = ei.accessor ;
                    }
                    return *this;
                }

                // ONLY detect curr.
                // ONLY use == with End() .
                bool operator == ( const EdgeIterator & ei )const
                {
                    return curr == ei.curr ;
                }

                // ONLY detect curr.
                // ONLY use == with End() .
                bool operator != ( const EdgeIterator & ei )const
                {
                    return curr != ei.curr ;
                }

                const Edge & operator*() const  { return *curr ; }

                const Edge * operator->() const  { return curr ; }

                EdgeIterator & operator ++() {
                    if( curr != NULL && accessor != NULL )
                    {
                        Id next = curr->next ;
                        if( next != Edge::invalid )
                            curr = &(accessor->AccessEdge(next, curr->from));
                        else
                            curr = NULL ;
                    }
                    else
                        curr = NULL ;
                    return *this ;
                }

                static EdgeIterator & end() 
                {
                    static EdgeIterator end;
                    return end ;
                }
                typename Edge::EdgeNodeId node_id ;
            private:
                const Edge * curr ;
                GraphAccess * accessor ;
        };


        template<class GraphAccess , class traits , class SearchNode  >
            struct PathEndHelperBase
            {
                typedef typename GraphAccess::GraphNodeId NodeId;
                typedef typename GraphAccess::GraphEdgeId EdgeId;
                typedef typename GraphAccess::Node        Node;
                typedef typename GraphAccess::Edge        Edge;
                typedef SearchNode                        SNode;
                typedef GraphAccess                       Access;
                typedef traits                            traisId;

                void Start() {assert(0);}
                void AddNode(const Node & , const SearchNode &) {assert(0);}
                void AddEdge(const Edge & ) {assert(0);}
                void PopEdge() {assert(0);}
                void PopNode() {assert(0);}
                bool IsEnd() const { assert(0) ;} ;

            };

    } // namespace GRAPH
} // namespace BGIQD

#endif //__ALGORITHM_GRAPH_GRAPH_H__
