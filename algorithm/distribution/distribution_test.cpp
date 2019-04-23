#include "algorithm/distribution/distribution.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(distribution)

TEST(DD)
{
    typedef BGIQD::DISTRIBUTION::IntervalDistribution<int> DD;
    DD d;
    d.Init(10,0,30);
    d.Count(0);
    d.Count(1);
    d.Count(2);
    d.Count(3);
    d.Count(13);
    d.Count(15);
    d.Count(23);
    d.Count(33);
    auto dd = d.CalcPercent();
    CHECK_FLOAT(float(4)/float(7) ,dd.GetPercent(5)); 
    CHECK_FLOAT(float(2)/float(7) ,dd.GetPercent(15)); 
    CHECK_FLOAT(float(1)/float(7) ,dd.GetPercent(25)); 
    CHECK_FLOAT(float(0)/float(7) ,dd.GetPercent(35)); 
    std::cerr<<dd.ToString();
    DD d1;
    d1.Init(10,0,30);
    d1.Count(1);
    d1.Count(11);
    auto d11 = d1.CalcPercent() ;
    auto dk = d11.ValidKeys();
    auto dt = dd.GetSubPercent(dk);
    CHECK_FLOAT(float(1)/float(3) ,dt.GetPercent(11));
    CHECK_FLOAT(float(2)/float(3) ,dt.GetPercent(1));
    std::cerr<<d11.ToString();
    std::cerr<<dt.ToString();
}

