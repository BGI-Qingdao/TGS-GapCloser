#include "algorithm/graph/DepthSearch.h"
#include "algorithm/graph/Graph.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(GraphDepthSearch)

//struct TestGraph
//{
//    BGIQD::SOAP2::GraphEA graph_ea ;
//    void Init() 
//    {
//        // init graph
//    }
//};

struct TestGraph1
{
    struct Node
    {
        char id ;
        int edge_id ;
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
     * }
     *
     * draw the graph by DOT if you want.
     *************************************************/

    static TestGraph1 GetTestGraph()
    {

        TestGraph1 test;

        test.add_edge(1,'u','v',2);
        test.add_edge(2,'u','x',-1);
        test.add_edge(3,'x','v',-1);
        test.add_edge(4,'v','y',-1);
        test.add_edge(5,'y','x',-1);
        test.add_edge(6,'w','y',7);
        test.add_edge(7,'w','z',-1);
        test.add_edge(8,'z','z',-1);

        test.add_node('u',1);
        test.add_node('x',3);
        test.add_node('v',4);
        test.add_node('y',5);
        test.add_node('w',6);
        test.add_node('z',8);

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

    void add_node( char id , int edge_id )
    {
        auto & node = nodes[id];
        node.id = id ;
        node.edge_id  = edge_id;
    }

};

struct ACCESS : public BGIQD::GRAPH::GraphAccessBase<TestGraph1 , char , int >
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
struct Ender1 : public BGIQD::GRAPH::PathEndHelperBase<ACCESS, traits_1 , BGIQD::GRAPH::DepthSearchNode<ACCESS::Node>>
{
    void Start() 
    {
        clean();
    }
    void AddNode(const Node & , const SNode & dnode) {
        pre_type =dnode.type ;
    }
    void AddEdge(const Edge & ) {
        clean();
    }

    void PopEdge() {clean();}
    void PopNode() {clean();}

    bool IsEnd() const {
        return ! ( pre_type == BGIQD::GRAPH::DepthSearchEdgeType::Invalid
            || pre_type == BGIQD::GRAPH::DepthSearchEdgeType::White );
    }

    private:
    BGIQD::GRAPH::DepthSearchEdgeType pre_type;
    void clean()
    {
        pre_type = BGIQD::GRAPH::DepthSearchEdgeType::Invalid;
    }
};


/*
 * Full depth search . END ONLY AT CIRCLE
 */
struct Ender2 : public BGIQD::GRAPH::PathEndHelperBase<ACCESS, traits_2, BGIQD::GRAPH::DepthSearchNode<ACCESS::Node>>
{
    void Start() 
    {
        clean();
    }
    void AddNode(const Node &n , const SNode & ) {
        if( now != -1 )
        {
            ids.insert( now ) ;
            s.push( now );
        }
        now = n.id ;
    }
    void AddEdge(const Edge & ) {

    }

    void PopEdge() {

    }

    void PopNode() {
        if( now != -1 )
            now = -1 ;
        else
        {
            NodeId prev = s.top() ;
            s.pop() ;
            ids.erase(prev);
        }
    }

    bool IsEnd() const {
        return now != -1 && ids.find( now ) != ids.end () ;
    }

    private:
    NodeId now ;
    void clean()
    {
        now = -1;
    }
    std::set<NodeId> ids;
    std::stack<NodeId> s;
};

TEST(GraphAccessNode)
{
    auto test = TestGraph1::GetTestGraph() ;

    ACCESS acc ;
    acc.base = &test;

    CHECK('u',acc.AccessNode('u').id);
    CHECK(1,acc.AccessNode('u').edge_id);

    CHECK('v',acc.AccessNode('v').id);
    CHECK(4,acc.AccessNode('v').edge_id);

    CHECK('x',acc.AccessNode('x').id);
    CHECK(3,acc.AccessNode('x').edge_id);

    CHECK('y',acc.AccessNode('y').id);
    CHECK(5,acc.AccessNode('y').edge_id);

    CHECK('w',acc.AccessNode('w').id);
    CHECK(6,acc.AccessNode('w').edge_id);

    CHECK('z',acc.AccessNode('z').id);
    CHECK(8,acc.AccessNode('z').edge_id);
}

TEST(GraphAccessEdge)
{
    auto test = TestGraph1::GetTestGraph() ;

    ACCESS acc ;
    acc.base = &test;

    CHECK(1  ,acc.AccessEdge(1,'u').id);
    CHECK('u',acc.AccessEdge(1,'u').from);
    CHECK('v',acc.AccessEdge(1,'u').to);
    CHECK(2  ,acc.AccessEdge(1,'u').next);

    CHECK(2  ,acc.AccessEdge(2,'u').id);
    CHECK('u',acc.AccessEdge(2,'u').from);
    CHECK('x',acc.AccessEdge(2,'u').to);
    CHECK(-1 ,acc.AccessEdge(2,'u').next);

    CHECK(5  ,acc.AccessEdge(5,'y').id);
    CHECK('y',acc.AccessEdge(5,'y').from);
    CHECK('x',acc.AccessEdge(5,'y').to);
    CHECK(-1 ,acc.AccessEdge(5,'y').next);

    CHECK(8  ,acc.AccessEdge(8,'z').id);
    CHECK('z',acc.AccessEdge(8,'z').from);
    CHECK('z',acc.AccessEdge(8,'z').to);
    CHECK(-1 ,acc.AccessEdge(8,'z').next);
}

TEST(EdgeItr_test)
{
    auto test = TestGraph1::GetTestGraph() ;
    ACCESS acc ;
    acc.base = &test;

    EdgeItr itr1(acc.AccessEdge(1,'u'),acc);
    CHECK( 1 , itr1->id );
    CHECK('u', itr1->from);
    CHECK('v', itr1->to);
    CHECK(2  , itr1->next);
    ++ itr1 ;

    CHECK( 2 , itr1->id );
    CHECK('u', itr1->from);
    CHECK('x', itr1->to);
    CHECK(-1  , itr1->next);
    ++ itr1 ;

    CHECK( true , (EdgeItr::end() == itr1 ));

    EdgeItr itr2(acc.AccessEdge(8,'z'),acc);
    CHECK( 8 , itr2->id );
    CHECK('z', itr2->from);
    CHECK('z', itr2->to);
    CHECK(-1 , itr2->next);
    ++ itr2 ;

    CHECK( true , (EdgeItr::end() == itr2 ));
}

typedef BGIQD::GRAPH::DepthSearch<ACCESS,EdgeItr,Ender1> Searcher1;
typedef BGIQD::GRAPH::DepthSearch<ACCESS,EdgeItr,Ender2> Searcher2;

TEST(GraphDepthSearch_test)
{
    auto test = TestGraph1::GetTestGraph() ;
    ACCESS acc ;
    acc.base = & test ;
    Searcher1 s1;
    s1.accesser = acc ;
    Ender1 ender ;
    s1.ender = ender;
    int end = s1.DoDepthSearch('u',1);
    s1.DoDepthSearch('w',end+1);
    auto & u = s1.nodes.at('u');
    auto & v = s1.nodes.at('v');
    auto & w = s1.nodes.at('w');
    auto & x = s1.nodes.at('x');
    auto & y = s1.nodes.at('y');
    auto & z = s1.nodes.at('z');

    CHECK(1,u.first_found);
    CHECK(8,u.finish_search);

    CHECK(2,v.first_found);
    CHECK(7,v.finish_search);
    CHECK(true , (v.backword_from.find('x') != v.backword_from.end()) )

    CHECK(3,y.first_found);
    CHECK(6,y.finish_search);
    CHECK(true , (y.cross_from.find('w') != y.cross_from.end()) )

    CHECK(4,x.first_found);
    CHECK(5,x.finish_search);
    CHECK(true , (x.forword_from.find('u') != z.forword_from.end()) )

    CHECK(9, w.first_found);
    CHECK(12,w.finish_search);

    CHECK(10,z.first_found);
    CHECK(11,z.finish_search);
    CHECK(true , (z.backword_from.find('z') != z.backword_from.end()) )


    s1.PrintNodes();
}

TEST(GraphDepthSearch_test_1)
{
    auto test = TestGraph1::GetTestGraph() ;
    ACCESS acc ;
    acc.base = & test ;
    Searcher2 s1;
    s1.accesser = acc ;
    Ender2 ender ;
    s1.ender = ender;
    int end = s1.DoDepthSearch('u',1);
    s1.DoDepthSearch('w',end+1);
/*
 * auto & u = s1.nodes.at('u');
    auto & v = s1.nodes.at('v');
    auto & w = s1.nodes.at('w');
    auto & x = s1.nodes.at('x');
    auto & y = s1.nodes.at('y');
    auto & z = s1.nodes.at('z');

    CHECK(1,u.first_found);
    CHECK(8,u.finish_search);

    CHECK(2,v.first_found);
    CHECK(7,v.finish_search);
    CHECK(true , (v.backword_from.find('x') != v.backword_from.end()) )

    CHECK(3,y.first_found);
    CHECK(6,y.finish_search);
    CHECK(true , (y.cross_from.find('w') != y.cross_from.end()) )

    CHECK(4,x.first_found);
    CHECK(5,x.finish_search);
    CHECK(true , (x.forword_from.find('u') != z.forword_from.end()) )

    CHECK(9, w.first_found);
    CHECK(12,w.finish_search);

    CHECK(10,z.first_found);
    CHECK(11,z.finish_search);
    CHECK(true , (z.backword_from.find('z') != z.backword_from.end()) )
*/
    s1.PrintNodes();
}
