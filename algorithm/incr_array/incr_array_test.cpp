#include "algorithm/incr_array/incr_array.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(IncrArray)

typedef BGIQD::INCRARRAY::IncrArray<int> IntArray;


IntArray & GetTestData()
{
    static IntArray test(10);

    if( test.empty() )
    {
        for( int i = 0 ; i < 88 ; i++ )
            test.push_back(i);
    }

    return test ;
}

TEST(IncrArrayAcess)
{
    auto & test = GetTestData() ;

    for( int i = 0 ; i < 88 ; i++ )
    {
        CHECK( i , test[i]);
    }
    CHECK(88 ,test.size() );
    CHECK(90 ,test.capacity() );
    int j = 0 ;
    for( auto i = test.begin() ; i != test.end() ; i++ )
    {
        CHECK( j , *i);
        j++ ;
    }
    CHECK( 88 , j );
    j = 0 ;

    for( auto i = test.begin() ; i < test.end() ; i+= 2 )
    {
        CHECK( j , *i);
        j+= 2 ;
    }
    CHECK( 88 , j );

    j = 0; 
    for( auto & i : test )
    {
        CHECK( j , i);
        j ++ ;
    }
    CHECK( 88 , j );
}

