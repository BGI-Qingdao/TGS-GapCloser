#include "common/test/Test.h"
#include "algorithm/graph/MinTree.h"
#include "algorithm/graph/GraphBasic.h"
TEST_MODULE_INIT(MinTree)


typedef BGIQD::GRAPH::IGraphNodeBasic<std::string , int > MTestNode ;

typedef BGIQD::GRAPH::IGraphEdgeBasic<std::string , int > MTestEdge;

struct MTEdge : public MTestEdge
{
    int value ;
    std::string ToString() const
    {
        std::ostringstream ost;
        ost<<from<<"\t--\t"<<to<<" [ id = "<<id << " value = "<<value<< " ]";
        return ost.str();
    }
};

struct EAttr
{
    int GetValue(const MTEdge & e ) const
    {
        return e.value ;
    }
};


struct MTestGraphBasic : public BGIQD::GRAPH::ListGraph<MTestNode , MTEdge>
{
    typedef BGIQD::GRAPH::ListGraph<MTestNode , MTEdge> Basic;
    void AddEdgeValue( const std::string & from , const std::string & to, int value )
    {
        MTEdge tmp ;
        tmp.from = from ;
        tmp.to = to ;
        tmp.value = value ;
        Basic::AddEdge(tmp);
    }

    static MTestGraphBasic TestData() {
        MTestGraphBasic test ;
        test.AddEdgeValue( "a" , "b",4);
        test.AddEdgeValue( "a" , "h",8);
        test.AddEdgeValue( "b" , "c",8);
        test.AddEdgeValue( "b" , "h",11);
        test.AddEdgeValue( "c" , "d",7);
        test.AddEdgeValue( "c" , "f",4);
        test.AddEdgeValue( "h" , "i" ,7);
        test.AddEdgeValue( "h" , "g" ,1);
        test.AddEdgeValue( "i" , "c" ,2);
        test.AddEdgeValue( "i" , "g" ,6);
        test.AddEdgeValue( "g" , "f" ,2);
        test.AddEdgeValue( "f" , "d" ,17);
        test.AddEdgeValue( "d" , "e" ,9);
        test.AddEdgeValue( "f" , "e" ,10);
        return test;
    }

};

typedef BGIQD::GRAPH::MinTreeHelper<MTestGraphBasic, int ,EAttr> MTHelper;

TEST(MinTree)
{
    auto test = MTestGraphBasic::TestData();
    MTHelper mtHelper;
    EAttr attr;
    auto m = mtHelper.MinTree(test,attr);
    m.PrintAsDOT(std::cout);
}

