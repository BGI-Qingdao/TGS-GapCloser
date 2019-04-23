#include "algorithm/align/SmithWaterman.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(SmithWatermanTest)

typedef BGIQD::ALIGN::SmithWaterman<char,std::string> Tester;
BGIQD::ALIGN::Schemes test_schemes{ 1 , 0 ,0 ,0 };
//
// by 
//  match = 1 
//  mismatch = 0 
//  insert = 0
//  delete = 0 
// 
// example 1
//      from
//          {"ABCBDAB"};
//          {"BDCABA"};
//      ==>
//          BCBA    1D1M1I1M1I1M2D1M1D
//          BDAB    1D1M3D1M1I2M1I
//          BCBA    1D1M1I1M1I1M1D1M1D
//
// example 2
//      from
//
//                GTCG  T CG  GAAGCCGGCCGAA 
//      ==>
//          {"ACCGGTCGAGTGCGCGGAAGCCGGCCGAA"};
//          {"GTCGTTCGGAATGCCGTTGCTCTGTAAA"};
//      ==>
//            GTCGT CGGAA GCCG  GC C G  AA 
//


TEST(SWTest01)
{
    Tester test01 ;
    test01.schemes = test_schemes ;
    test01.ref = "ABCBDAB";
    test01.query = "BDCABA" ;
    test01.CheckLoadedData();
    test01.InitAfterDataLoaded();
    test01.FillMutrix();
    auto path = test01.GetResult();
    auto m1 = test01.AlignedElementsRef(path);
    auto m2 = test01.AlignedElementsQuery(path);

    CHECK("BCAB" ,m1);
    CHECK("BCAB" ,m2);
}

TEST(SWTest02)
{
    Tester test01 ;
    test01.schemes = test_schemes ;
    test01.ref = "ACCGGTCGAGTGCGCGGAAGCCGGCCGAA";
    test01.query = "GTCGTTCGGAATGCCGTTGCTCTGTAAA" ;
    test01.CheckLoadedData();
    test01.InitAfterDataLoaded();
    test01.FillMutrix();
    auto path = test01.GetResult();
    auto m1 = test01.AlignedElementsRef(path);
    auto m2 = test01.AlignedElementsQuery(path);

    CHECK("GTCGTCGGAAGCCGGCCGAA" ,m1);
    CHECK("GTCGTCGGAAGCCGGCCGAA" ,m2);
}
