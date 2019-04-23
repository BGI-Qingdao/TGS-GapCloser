#ifndef __ALGORITHM_GRAPH_SPFSEARCH_H__
#define __ALGORITHM_GRAPH_SPFSEARCH_H__
#include <map>
#include <sstream>
#include <iostream>
#include <set>
#include "algorithm/graph/Graph.h"
#include "algorithm/fibheap/fib_heap.h"

namespace BGIQD{
    namespace GRAPH{

        template<class BaseNode>
            struct SPFNode
            {
                typedef typename BaseNode::NodeNodeId NodeId;
                typedef BGIQD::FIBHEAP::Node<int,typename BaseNode::NodeNodeId> FibBaseNode;
                enum Type
                {
                    Unknow = 0 ,
                    InHeap = 1 ,
                    Finish = 2 ,
                };
                Type type;

                FibBaseNode Base;
                void InitAsRoot(const BaseNode & node )
                {
                    Base.value = node.id ;
                    Base.key = 0;
                    type = Type::Finish ;
                }
                void InitNode(const BaseNode & node ,const SPFNode & father , int length)
                {
                    Base.value = node.id ;
                    Base.key = father.Base.key + length;
                    type = Type::InHeap ;
                    prev = father.Base.value;
                }
                void ResetParent(const BaseNode & ,const SPFNode & father , int )
                {
                    other_from.insert(prev);
                    prev = father.Base.value;
                }

                void TouchParent( const BaseNode & ,const SPFNode & father , int )
                {
                    other_from.insert(father.Base.value);
                }
                std::set<NodeId>  other_from;
                NodeId prev ;
                std::string ToString() const {
                    std::ostringstream ost;
                    ost<<"id = "<<Base.value<<" prev= "<<prev;
                    ost<<" path_length = "<<Base.key;
                    ost<<" other from = ";
                    for( auto i:other_from)
                    {
                        ost<<i<<'\t';
                    }
                    return ost.str();
                }
            };

        template<class GraphAccess
            , class EdgeItr
            , class PathEnder
            , class SPFNode1 = SPFNode<typename GraphAccess::Node > >
            struct SPFSearch 
            {
                typedef typename GraphAccess::GraphNodeId   NodeId;
                typedef typename GraphAccess::GraphEdgeId   EdgeId;
                typedef typename GraphAccess::Node          NodeBase;
                typedef typename GraphAccess::Edge          EdgeBase;

                typedef SPFNode1                            SPFNode;
                typedef typename SPFNode1::FibBaseNode      FibNode;
                typedef BGIQD::FIBHEAP::FibHeap<FibNode>    FibHeap;

                void DoSPFSearch(NodeId start)
                {
                    NodeBase & root = accesser.AccessNode(start);

                    //path.push(EdgeItr(accesser.AccessEdge(root.edge_id , root.id) , accesser));
                    //ender.AddEdge(accesser.AccessEdge(root.edge_id));
                    ender.Start();
                    NodeId prev ;
                    fib_nodes[start].InitAsRoot(root);
                    heap.Insert(fib_nodes[start].Base);
                    while ( ! heap.Empty() )
                    {
                        auto & min = heap.ExtractMin() ;
                        auto & root = accesser.AccessNode( min.value) ;
                        EdgeItr edge_itr(accesser.AccessEdge(root.edge_id , root.id) , accesser);
                        if( edge_itr != EdgeItr::end())
                            AddEdge(edge_itr);
                    }
                }

                PathEnder ender;

                GraphAccess accesser;

                void PrintNodes()
                {
                    for( auto & i : fib_nodes )
                    {
                        std::cerr<<i.second.ToString()<<std::endl;
                    }
                };

                    std::map<NodeId ,SPFNode> fib_nodes;
                protected:
                    FibHeap heap;

                    void AddEdge(EdgeItr & itr)
                    {
                        NodeId from = itr->from;
                        SPFNode * f_node = NULL ;
                        try{
                            f_node = &fib_nodes.at(from);
                        }
                        catch ( ... )
                        {
                            assert(0);
                        }
                        assert(f_node);

                        f_node->type = SPFNode::Type::Finish ;

                        while( itr != EdgeItr::end() )
                        {
                            ender.AddEdge(*itr);
                            if( ender.IsEnd() )
                            {
                                ender.PopEdge();
                                ++ itr ;
                                continue ;
                            }
                            ender.PopEdge();
                            NodeId to = itr->to ;
                            auto & base_to_node = accesser.AccessNode(to);
                            int l = -1;
                            accesser.GetAttr(base_to_node,*itr,"length",l);
                            assert( l >= 0 );
                            ++ itr ;
                            auto itr_n = fib_nodes.find( to );
                            if( itr_n == fib_nodes.end() )
                            {
                                // This is a new node ;
                                auto & new_node = fib_nodes[to];
                                new_node.InitNode(base_to_node,*f_node,l);
                                ender.AddNode(base_to_node ,new_node);
                                if( ender.IsEnd() )
                                {
                                    ender.PopNode() ;
                                    continue ;
                                }
                                heap.Insert(new_node.Base);
                                // pop it because push history is useless .
                                ender.PopNode();
                            }
                            else
                            {
                                auto & node = itr_n->second;
                                if( f_node->Base.key + l < node.Base.key )
                                {
                                    assert( node.type == SPFNode::Type::InHeap );
                                    heap.DecreaseKey(node.Base , f_node->Base.key+l);
                                    node.ResetParent(base_to_node,*f_node,l);
                                }
                                else
                                    node.TouchParent(base_to_node,*f_node,l);
                            }
                        }
                    }
            };
    }
}
#endif //__ALGORITHM_GRAPH_SPFSEARCH_H__
