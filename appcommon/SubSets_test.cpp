#include "common/test/Test.h"
#include "appcommon/SubSets.h"
TEST_MODULE_INIT(Subsets)

TEST(Subsets01)
{
    int s , e ;
    BGIQD::SUBSETS::SubSets test01;
    //////////////////////////////////
    CHECK(false , test01.Pop(s,e));
    //////////////////////////////////
    s=10,e=20;
    test01.Push(s,e);
    s=0,e=0;
    CHECK(true, test01.Pop(s,e));
    CHECK(10,s);
    CHECK(20,e);

    //////////////////////////////////
    int array[2][2] =
    {
        { 1 , 10 } ,
        { 20, 30 }
    };
    for( int i = 0 ; i < 2 ; i ++ )
    {
        test01.Push(array[i][0] , array[i][1] );
    }

    s=0,e=0;
    CHECK(true, test01.Pop(s,e));
    CHECK(1,s);
    CHECK(10,e);
    s=0,e=0;
    CHECK(true, test01.Pop(s,e));
    CHECK(20,s);
    CHECK(30,e);
    CHECK(false, test01.Pop(s,e));
    //////////////////////////////////
    int array1[8][2] =
    {
        { 1 , 10 } ,
        { 20, 30 } ,
        { 40, 60 } ,
        { 90, 100 } ,
        { 20, 30 } ,
        { 50, 55 } ,
        { 21, 35 } ,
        { 33, 45 } ,
    };
    for( int i = 0 ; i < 8 ; i ++ )
    {
        test01.Push(array1[i][0] , array1[i][1] );
    }

    s=0,e=0;
    CHECK(true, test01.Pop(s,e));
    CHECK(1,s);
    CHECK(10,e);
    s=0,e=0;
    CHECK(true, test01.Pop(s,e));
    CHECK(20,s);
    CHECK(60,e);
    s=0,e=0;
    CHECK(true, test01.Pop(s,e));
    CHECK(90,s);
    CHECK(100,e);
    CHECK(false, test01.Pop(s,e));
    //////////////////////////////////
}

