#include "common/test/Test.h"
#include "algorithm/graph/SPFSearch.h"

TEST_MODULE_INIT(SPFSearchTest)

struct TestGraph
{
    struct Node
    {
        char id ;
        int edge_id ;
        int length ;
    };

    struct Edge
    {
        char from ; 
        char to ; 
        int next ;
    };

    std::map< char , Node> nodes ;
    std::map< int , Edge> edges ;

    /**************************************************
     * Test graph :
     *
     * digraph{
     *  'u'->'v'
     *  'u'->'x'
     *  'x'->'v'
     *  'v'->'y'
     *  'y'->'x'
     *  'w'->'y'
     *  'w'->'z'
     *  'z'->'z'
     *  'u'->'w'
     * }
     *
     * draw the graph by DOT if you want.
     *************************************************/

    static TestGraph GetTestGraph()
    {

        TestGraph test;

        test.add_edge(1,'u','v',2);
        test.add_edge(2,'u','x',9);
        test.add_edge(3,'x','v',-1);
        test.add_edge(4,'v','y',-1);
        test.add_edge(5,'y','x',-1);
        test.add_edge(6,'w','y',7);
        test.add_edge(7,'w','z',-1);
        test.add_edge(8,'z','z',-1);
        test.add_edge(9,'u','w',-1);

        test.add_node('u',1,1);
        test.add_node('x',3,2);
        test.add_node('v',4,6);
        test.add_node('y',5,1);
        test.add_node('w',6,4);
        test.add_node('z',8,3);

        return test ;
    }

    private :

    void add_edge( int id , char from , char to , int next )
    {
        auto & edge3 = edges[id] ;
        edge3.from = from;
        edge3.to= to;
        edge3.next = next ;
    }

    void add_node( char id , int edge_id , int l)
    {
        auto & node = nodes[id];
        node.id = id ;
        node.edge_id  = edge_id;
        node.length = l;
    }

};

struct L_NodeBase : public BGIQD::GRAPH::GraphNodeBase<char , int>
{
    int length;
};

struct ACCESS : public BGIQD::GRAPH::GraphAccessBase<
                TestGraph 
                , char 
                , int
                , L_NodeBase
                >
{
    //typedef char                         GraphNodeId ;
    //typedef int                          GraphEdgeId ;

    //typedef GraphNodeBase<char ,int>     Node;
    //typedef GraphEdgeBase<char ,int>     Edge;

    Node & AccessNode(GraphNodeId i)
    {
        auto itr = nodes.find(i); 
        if( itr == nodes.end() )
        {
            auto & n = nodes[i] ;
            n.id = (*base).nodes.at(i).id ;
            n.edge_id = (*base).nodes.at(i).edge_id ;
            n.length = (*base).nodes.at(i).length;
            return n;
        }
        return itr->second ;
    }

    Edge & AccessEdge(GraphEdgeId i , GraphNodeId )
    {
        auto itr = edges.find(i); 
        if( itr == edges.end() )
        {
            auto & n = edges[i] ;
            auto & base_edge = base->edges.at(i);
            n.id = i ;
            n.from = base_edge.from;
            n.to   = base_edge.to;
            n.next = base_edge.next;
            return n;
        }
        return itr->second ;
    }
    void GetAttr( const Node & n , const Edge & e , const std::string & name , int & l)
    {
        l = n.length ;
    }

//    TestGraph * base ;
//    std::map< char, Node> nodes ;
//    std::map< int , Edge> edges ;
};

struct traits_1 {} ;
struct traits_2 {} ;

//typedef BGIQD::GRAPH::GraphAccess<TestGraph,char,int> ACCESS;
typedef BGIQD::GRAPH::EdgeIterator<ACCESS> EdgeItr;

/*
 * Classic depth search . Only end at NOT WHITE node
 */
struct Ender1 : public BGIQD::GRAPH::PathEndHelperBase<ACCESS, traits_1 , BGIQD::GRAPH::SPFNode<ACCESS::Node>>
{
    void Start() 
    {
        clean();
    }
    void AddNode(const Node &n , const SNode & dnode) {
    }
    void AddEdge(const Edge & ) {
        clean();
    }

    void PopEdge() {clean();}
    void PopNode() {clean();}

    bool IsEnd() const {
        return end_flag;
    }

    private:
    bool end_flag;
    void clean()
    {
        end_flag = false ;
    }
};

typedef BGIQD::GRAPH::SPFSearch<ACCESS,EdgeItr,Ender1> Searcher1;

TEST( SPFSearch )
{
    auto test = TestGraph::GetTestGraph() ;
    ACCESS acc ;
    acc.base = & test ;
    Searcher1 s1;
    s1.accesser = acc ;
    Ender1 ender ;
    s1.ender = ender;
    s1.DoSPFSearch('u');
    s1.PrintNodes();

    auto & u = s1.fib_nodes.at('u');
    auto & v = s1.fib_nodes.at('v');
    auto & w = s1.fib_nodes.at('w');
    auto & x = s1.fib_nodes.at('x');
    auto & y = s1.fib_nodes.at('y');
    auto & z = s1.fib_nodes.at('z');

    CHECK(0,u.Base.key);
    //CHECK(8,u.finish_search);

    CHECK(6,v.Base.key);
    CHECK('u',v.prev);
    CHECK(true , (v.other_from.find('x') != v.other_from.end()) )

    CHECK(5,y.Base.key);
    CHECK('w',y.prev);
    CHECK(true , (y.other_from.find('v') != y.other_from.end()) )

    CHECK(2,x.Base.key);
    CHECK('u',x.prev);
    CHECK(true , (x.other_from.find('y') != z.other_from.end()) )

    CHECK(4, w.Base.key);
    CHECK('u',w.prev);

    CHECK(7,z.Base.key);
    CHECK('w',z.prev);
    CHECK(true , (z.other_from.find('z') != z.other_from.end()) )
}

