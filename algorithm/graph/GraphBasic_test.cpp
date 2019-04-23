#include "common/test/Test.h"
#include "algorithm/graph/GraphBasic.h"

TEST_MODULE_INIT(GraphBasicTest)

typedef BGIQD::GRAPH::IGraphNodeBasic<std::string , int > TestNode ;

typedef BGIQD::GRAPH::IGraphEdgeBasic<std::string , int > TestEdge;

typedef BGIQD::GRAPH::IDigraphEdgeBase<std::string , int > TestDiEdge;

struct TestGraphBasic : public BGIQD::GRAPH::ListGraph<TestNode , TestEdge>
{
    static TestGraphBasic TestData() {
        TestGraphBasic test ;
        test.AddEdge( "NodeA" , "NodeB");
        test.AddEdge( "NodeA" , "NodeC");
        test.AddEdge( "NodeB" , "NodeC");
        test.AddEdge( "NodeB" , "NodeD");
        test.AddEdge( "NodeC" , "NodeD");

        return test;
    }
};

struct TestDigraphBasic : public BGIQD::GRAPH::ListDigraph<TestNode , TestDiEdge>
{
    static TestDigraphBasic TestData() {
        TestDigraphBasic test ;
        test.AddEdge( "NodeA" , "NodeB");
        test.AddEdge( "NodeA" , "NodeC");
        test.AddEdge( "NodeB" , "NodeC");
        test.AddEdge( "NodeB" , "NodeD");
        test.AddEdge( "NodeC" , "NodeD");
        return test;
    }
};


TEST(ListGraph)
{
    auto  test = TestGraphBasic::TestData();
    test.PrintAsDOT(std::cout);
    CHECK( true ,test.CheckEdge("NodeA","NodeB") ) 
    CHECK( true ,test.CheckEdge("NodeB","NodeA") ) 
    CHECK( true , test.GetNode("NodeA").HasEdge(0));
    CHECK( true , test.GetNode("NodeB").HasEdge(0));
}

TEST(ListDigraph)
{
    auto  test = TestDigraphBasic::TestData();
    test.PrintAsDOT(std::cout);
    CHECK( true ,test.CheckEdge("NodeA","NodeB") ) 
    CHECK( false ,test.CheckEdge("NodeB","NodeA") ) 
    CHECK( true , test.GetNode("NodeA").HasEdge(0));
    CHECK( false, test.GetNode("NodeB").HasEdge(0));
}

