#include "algorithm/collection/collection.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(collectionTest)

typedef BGIQD::Collection::Collection<char> CharCollect;

CharCollect InitC1()
{
    CharCollect test;
    test.IncreaseElement('H');
    test.IncreaseElement('e');
    test.IncreaseElement('l');
    test.IncreaseElement('l');
    test.IncreaseElement('o');
    return test;
}

CharCollect InitC2()
{
    CharCollect test;
    test.IncreaseElement('W');
    test.IncreaseElement('o');
    test.IncreaseElement('r');
    test.IncreaseElement('l');
    test.IncreaseElement('d');
    return test;
}

TEST(CollectSize)
{
    CharCollect test ;
    test += InitC1() ;
    test += InitC2() ;
    CHECK( 10 , test.size() );
    CHECK( 7 , test.keysize() );
}

TEST(Union)
{
    CharCollect t1 = InitC1();
    CharCollect t2 = InitC2();
    auto t3 = CharCollect::Union(t1,t2);
    CHECK ( 8 , t3.size() );
    CHECK ( 7 , t3.keysize() );
};


TEST(Intersection)
{
    CharCollect t1 = InitC1();
    CharCollect t2 = InitC2();
    auto t3 = CharCollect::Intersection(t1,t2);
    CHECK ( 2 , t3.size() );
    CHECK ( 2 , t3.keysize() );
};

TEST(Jaccard)
{
    CharCollect t1 = InitC1();
    CharCollect t2 = InitC2();
    CHECK( 0.25 , CharCollect::Jaccard(t1,t2) );
}

TEST(CollectIterator)
{
    std::map<char , int > t;
    t['H'] = 1 ;
    t['e']= 1;
    t['l']=2;
    t['o']=1;
    auto t1 = InitC1();
    for( auto i : t1 )
    {
        std::cerr<<i.first<<'\t'<<i.second<<std::endl;
        CHECK(t[i.first] , i.second);
    }
    std::cerr<<std::endl;
}
