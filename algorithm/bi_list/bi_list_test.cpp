#include "algorithm/bi_list/bi_list.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(BiList)

typedef BGIQD::BILIST::BiList<int> TestBiList;

TEST(BiListInsert)
{
    TestBiList list[10];
    int test_array[10];
    for( int i = 0 ; i < 10 ; i ++ )
    {
        test_array[i] = i ;
        list[i].Init(&test_array[i]);
    }

    for( int i = 1 ; i < 5 ; i ++ )
        list[0].Insert(&list[i]);
    
    for( int i = 6 ; i < 10 ; i ++ )
        list[5].Insert(&list[i]);
    list[0].Insert(&list[5]);
    // Left first
    //print 9 8 7 6 5 4 3 2 1 0
    auto ptr = list[0].Backword(); ;
    int i =  9;
    while(true)
    {
       std::cerr<<*(ptr->self)<<'\t';
       CHECK( i , *(ptr->self) );
       i -- ;
       if( ptr == &list[0] )
           break;
       ptr = ptr->Backword();
    }
    std::cerr<<std::endl;
    // Right second
    //print 1 2 3 4 5 6 7 8 9 0
    ptr = list[0].Forward() ;
    i =  1;
    while(true)
    {
       std::cerr<<*(ptr->self)<<'\t';
       CHECK( i , *(ptr->self) );
       i ++ ;
       if ( i == 10 )
           i = 0 ;
       if( ptr == &list[0] )
           break;
       ptr = ptr->Forward();
    }
    std::cerr<<std::endl;
}

TEST(BiListDel)
{
    TestBiList list[10];
    int test_array[10];
    for( int i = 0 ; i < 10 ; i ++ )
    {
        test_array[i] = i ;
        list[i].Init(&test_array[i]);
    }

    for( int i = 1 ; i < 10 ; i ++ )
        list[0].Insert(&list[i]);

    list[0].DeleteMe();
    list[7].DeleteMe();
    //Left first
    //print 9 8 6 5 4 3 2 1
    auto ptr = list[1].Backword(); 
    int i =  9;
    while( true )
    {
       std::cerr<<*(ptr->self)<<'\t';
       CHECK( i , *(ptr->self) );
       i -- ;
       if( i == 7 )
           i -- ;
       if( ptr == &list[1] )
           break;
       ptr = ptr->Backword() ;
    }
    std::cerr<<std::endl;
    //Right second
    //print 2 3 4 5 6 8 9 1
    ptr = list[1].Forward(); 
    i =  2;
    while( true )
    {
       std::cerr<<*(ptr->self)<<'\t';
       CHECK( i , *(ptr->self) );
       i ++ ;
       if( i == 7 )
           i ++ ;
       else if ( i == 10 )
           i = 1 ;
       if( ptr == &list[1] )
           break;
       ptr = ptr->Forward();
    }
    std::cerr<<std::endl;
}
