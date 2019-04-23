#ifndef __ALGORITHM_GRAPHTRUNK_H__
#define __ALGORITHM_GRAPHTRUNK_H__

#include <map>
#include <set>
#include <cassert>
#include <vector>

namespace BGIQD{
    namespace GRAPH{

        template<class TEdge>
            struct TrunkNode
            {
                typedef typename TEdge::EdgeNodeId NodeId;
                typedef typename TEdge::EdgeEdgeId EdgeId;
                std::set<EdgeId> edges;

                int level;
                NodeId prev;
                NodeId prev_2;
                NodeId id;
                bool marked;
            };

        template<class TListGraph ,
            class TNode = TrunkNode<typename TListGraph::Edge> 
                >
                struct TrunkHelper
                {
                    typedef TListGraph ListGraph ;
                    typedef TNode TrunkNode ;

                    typedef typename TListGraph::Node Node;
                    typedef typename TListGraph::Edge Edge;
                    typedef typename Edge::EdgeEdgeId EdgeId;
                    typedef typename Edge::EdgeNodeId NodeId;


                    // Make sure input is a trunk
                    static std::vector<NodeId> LinearTrunk(const ListGraph &  base )
                    {
                        std::vector<NodeId> ret ;
                        NodeId starter ;
                        for( const auto & pair : base.nodes )
                        {
                            auto & node = pair.second ;
                            if( node.edge_ids.size() == 1 )
                            {
                                starter = node.id ;
                                break;
                            }
                            else
                            {
                                assert( node.edge_ids.size() == 2 );
                            }
                        }
                        ret.push_back( starter ) ;

                        auto & node = base.GetNode(starter);;
                        auto edge_id = *(node.edge_ids.begin());
                        auto & edge = base.GetEdge(edge_id);
                        NodeId next = edge.OppoNode(starter);
                        NodeId curr = starter;
                        while(1)
                        {
                            auto & next_node = base.GetNode(next) ;
                            ret.push_back(next);
                            if( next_node.edge_ids.size() == 1 )
                            {
                                break;
                            }
                            else
                            {
                                assert( next_node.edge_ids.size() == 2 );
                                auto edge_id1 = *(next_node.edge_ids.begin());
                                auto edge_id2 = *(std::next(next_node.edge_ids.begin()));
                                auto &edge1 = base.GetEdge(edge_id1);
                                auto &edge2 = base.GetEdge(edge_id2);
                                if( edge1.OppoNode(next) != curr)
                                {
                                    curr = next ;
                                    next = edge1.OppoNode(next) ;
                                }
                                else
                                {
                                    assert(edge2.OppoNode(next) != curr);
                                    curr = next ;
                                    next = edge2.OppoNode(next);
                                }
                            }
                        }
                        return ret ;
                    }

                    // Make sure input is a mintree 
                    static ListGraph  Trunk(const ListGraph &  base )
                    {
                        if( base.nodes.size() < 3 )
                            return base ;
                        std::map<NodeId, TrunkNode> nodes;

                        for( const auto & pair  : base.nodes )
                        {
                            const auto & node = pair.second ;
                            auto & new_node = nodes[node.id] ;
                            new_node.id= node.id ;
                            new_node.edges = node.edge_ids;
                            new_node.level = -1 ;
                            new_node.marked = false ;
                        }

                        ListGraph ret ;
                        auto add_node_into_ret= [&]( NodeId i) -> void
                        {
                            if(  ret.HasNode(i) )
                                return ;
                            const auto & base_node = base.GetNode(i) ;
                            ret.AddNode(base_node);
                            ret.GetNode(i).edge_ids.clear();
                        };

                        //Delete single
                        std::set<NodeId> single;
                        for( auto & pair : nodes )
                        {
                            auto & node = pair.second;
                            if( node.edges.size() == 0 )
                            {
                                single.insert(node.id);
                            }
                            if( node.edges.size() == 1 )
                            {
                                node.level = 1 ;
                            }
                        }
                        for( const auto id : single )
                        {
                            nodes.at(id).marked = true ;
                        }
                        // iterator delele leap
                        size_t count = nodes.size() - single.size() ;
                        while( count > 1 )
                        {
                            std::set<NodeId> candidate;
                            count = 0 ;
                            for( const auto & pair : nodes )
                            {
                                const auto & node = pair.second;
                                if( !node.marked )
                                {
                                    count ++ ;
                                    if( node.edges.size() == 1 )
                                    {
                                        candidate.insert(node.id) ;
                                    }
                                }

                            }
                            // All can be a root 
                            if(candidate.size() == count  )
                            {
                                assert( count == 2 );
                                break ;
                            }
                            for( const auto i : candidate )
                            {
                                auto & node = nodes.at(i) ;
                                EdgeId edge = *node.edges.begin();
                                const Edge & base_edge = base.GetEdge(edge);
                                NodeId oppo = base_edge.OppoNode(i);
                                if( candidate.find(oppo) != candidate.end() )
                                {
                                    continue ;
                                }

                                auto & oppo_node = nodes.at(oppo);
                                oppo_node.level = node.level + 1;
                                oppo_node.prev_2 = oppo_node.prev;
                                oppo_node.prev = node.id;
                                oppo_node.edges.erase(edge);
                            }
                            for( const auto i : candidate )
                            {
                                nodes.at(i).marked = true ;
                            }
                            count -= candidate.size() ;
                        }

                        std::set<NodeId> seeds;
                        std::set<NodeId> nexts;
                        for( const auto & pair : nodes )
                        {
                            const auto & node = pair.second;
                            if( !node.marked )
                            {
                                seeds.insert(node.id) ;
                            }
                        }

                        if( seeds.size() == 1 )
                        {
                            auto root = *seeds.begin();
                            add_node_into_ret(root);
                            auto  root_node = nodes.at(root);
                            nexts.insert(root_node.prev);
                            nexts.insert(root_node.prev_2);
                        }
                        else if( seeds.size() == 2 )
                        {
                            auto root1 = *seeds.begin();
                            auto root2 = *(std::next(seeds.begin()));
                            add_node_into_ret(root1);
                            add_node_into_ret(root2);
                            nexts.insert(nodes.at(root1).prev);
                            nexts.insert(nodes.at(root2).prev);
                            for( auto i : nodes.at(root1).edges)
                            {
                                const Edge & base_edge = base.GetEdge(i);
                                if( base_edge.OppoNode(root1) == root2 )
                                {
                                    ret.AddEdge(base_edge);
                                }
                            }
                        }
                        else
                        {
                            assert(0);
                        }

                        while(!nexts.empty())
                        {
                            assert(nexts.size() == 2);
                            auto next1 = *nexts.begin();
                            auto next2 = *(std::next(nexts.begin()));
                            const auto & next1_node = nodes.at(next1);
                            const auto & next2_node = nodes.at(next2);
                            assert(next1_node.edges.size() == 1 );
                            assert(next2_node.edges.size() == 1 );
                            assert(next2_node.level == next1_node.level);
                            add_node_into_ret(next1);
                            add_node_into_ret(next2);
                            ret.AddEdge(base.GetEdge(*next1_node.edges.begin()));
                            ret.AddEdge(base.GetEdge(*next2_node.edges.begin()));
                            nexts.clear();
                            if( next1_node.level > 1 )
                            {
                                nexts.insert(next1_node.prev) ;
                                nexts.insert(next2_node.prev) ;
                            }
                        }

                        return ret ;
                    }
                };
    }
}
#endif //__ALGORITHM_GRAPHTRUNK_H__
