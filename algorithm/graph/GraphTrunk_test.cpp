#include "common/test/Test.h"
#include "algorithm/graph/GraphTrunk.h"
#include "algorithm/graph/GraphBasic.h"

TEST_MODULE_INIT(MinTreeTrunk)

typedef BGIQD::GRAPH::IGraphNodeBasic<std::string , int > TKTestNode ;

typedef BGIQD::GRAPH::IGraphEdgeBasic<std::string , int > TKTestEdge;

struct TKEdge : public TKTestEdge
{
    int value ;
    std::string ToString() const
    {
        std::ostringstream ost;
        ost<<from<<"\t--\t"<<to<<" [ id = "<<id << " value = "<<value<< " ]";
        return ost.str();
    }
};

struct TKTestGraphBasic : public BGIQD::GRAPH::ListGraph<TKTestNode , TKEdge>
{
    typedef BGIQD::GRAPH::ListGraph<TKTestNode , TKEdge> Basic;
    void AddEdgeValue( const std::string & from , const std::string & to, int value )
    {
        TKEdge tmp ;
        tmp.from = from ;
        tmp.to = to ;
        tmp.value = value ;
        Basic::AddEdge(tmp);
    }

    static TKTestGraphBasic TestData1() {
        TKTestGraphBasic test ;

        test.AddEdgeValue( "a" , "b",4);
        test.AddEdgeValue( "a" , "h",8);
        test.AddEdgeValue( "c" , "d",7);
        test.AddEdgeValue( "c" , "f",4);
        test.AddEdgeValue( "h" , "g" ,1);
        test.AddEdgeValue( "i" , "c" ,2);
        test.AddEdgeValue( "g" , "f" ,2);
        test.AddEdgeValue( "d" , "e" ,9);
        return test;
    }
    static TKTestGraphBasic TestData2() {
        TKTestGraphBasic test ;

        test.AddEdgeValue( "a" , "h",8);
        test.AddEdgeValue( "c" , "d",7);
        test.AddEdgeValue( "c" , "f",4);
        test.AddEdgeValue( "h" , "g" ,1);
        test.AddEdgeValue( "h" , "m" ,1);
        test.AddEdgeValue( "i" , "c" ,2);
        test.AddEdgeValue( "g" , "f" ,2);
        test.AddEdgeValue( "d" , "e" ,9);
        return test;
    }

};

typedef BGIQD::GRAPH::TrunkHelper<TKTestGraphBasic> TKHelper;

TEST(MinTree_test1)
{
    auto test = TKTestGraphBasic::TestData1();
    auto m = TKHelper::Trunk(test);
    m.PrintAsDOT(std::cout);
    auto linear = TKHelper::LinearTrunk(m);
    for(const auto & x : linear)
    {
        std::cout<<'\t'<<x;
    }
    std::cout<<std::endl;
}

TEST(MinTree_test2)
{
    auto test = TKTestGraphBasic::TestData2();
    auto m = TKHelper::Trunk(test);
    m.PrintAsDOT(std::cout);
    auto linear = TKHelper::LinearTrunk(m);
    for(const auto & x : linear)
    {
        std::cout<<'\t'<<x;
    }
    std::cout<<std::endl;
}
