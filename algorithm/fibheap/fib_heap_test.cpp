#include "common/test/Test.h"
#include "algorithm/fibheap/fib_heap.h"

TEST_MODULE_INIT(FibHeap)


typedef BGIQD::FIBHEAP::Node<int , int > testFibHeapNode;
typedef BGIQD::FIBHEAP::FibHeap<testFibHeapNode> testFibHeap;
TEST(FibCommon)
{
    testFibHeapNode array[10];
    for( int i = 0 ; i < 10 ; i ++ )
    {
        array[i].key = std::rand() % 100 ;
        array[i].value = array[i].key * 2  ;
    }

    testFibHeap test;
    for( int i = 0 ; i < 10 ; i ++ )
    {
        test.Insert(array[i]);
    }
    int prev = -1 ;
    while( ! test.Empty() )
    {
        auto & min1 = test.Min() ;
        auto & min = test.ExtractMin() ;
        CHECK(true , (prev <= min.key )) ;
        CHECK(min1.key , min.key) ;
        std::cout<<min.key<<'\t'<<min.value<<std::endl;
        prev= min.key ;
    }
}

TEST(FibUnoin)
{
    testFibHeapNode array[10];
    for( int i = 0 ; i < 10 ; i ++ )
    {
        array[i].key = std::rand() % 100 ;
        array[i].value = array[i].key * 2  ;
    }

    testFibHeap test1;
    testFibHeap test2;
    for( int i = 0 ; i < 5 ; i ++ )
    {
        test1.Insert(array[i]);
    }

    for( int i = 5 ; i < 10 ; i ++ )
    {
        test2.Insert(array[i]);
    }
    testFibHeap test = testFibHeap::Union(test1,test2);
    int prev = -1 ;
    while( ! test.Empty() )
    {
        auto & min1 = test.Min() ;
        auto & min = test.ExtractMin() ;
        CHECK(true , (prev <= min.key )) ;
        CHECK(min1.key , min.key) ;
        std::cout<<min.key<<'\t'<<min.value<<std::endl;
        prev= min.key ;
    }
}


TEST(FibDecrese)
{
    testFibHeapNode array[10];
    for( int i = 0 ; i < 10 ; i ++ )
    {
        array[i].key = std::rand() % 100 ;
        array[i].value = array[i].key ;
    }

    testFibHeap test;
    for( int i = 0 ; i < 10 ; i ++ )
    {
        test.Insert(array[i]);
    } 
    auto & t = test.ExtractMin();
    test.Insert(t);

    for( int i = 0 ; i < 10 ; i += 2 )
    {
        test.DecreaseKey( array[i] , array[i].key / ((i%3) + 1));
    }
    int prev = -1 ;
    while( ! test.Empty() )
    {
        auto & min = test.ExtractMin() ;
        CHECK(true , (prev <= min.key )) ;
        std::cout<<min.key<<'\t'<<min.value<<std::endl;
        prev= min.key ;
    }
}
