#include "algorithm/linear_fitting/Minimum_multiplication.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(MMM)

typedef BGIQD::LINEARFITTING::Item<int, int> Point;

TEST(MMM_11)
{
    std::vector<Point> data;
    data.push_back(Point{ 0,0});
    data.push_back(Point{ 1,1});
    data.push_back(Point{ 2,2});
    data.push_back(Point{ 3,3});
    auto ret = BGIQD::LINEARFITTING::lineFit(data);
    CHECK_FLOAT(1.0f,ret.a/ret.b);
    CHECK_FLOAT(0.0f,ret.c);
    CHECK(4,ret.getY(4));
    CHECK(4,ret.getX(4));
}


TEST(MMM_12)
{
    std::vector<Point> data;
    data.push_back(Point{ 0,0});
    data.push_back(Point{ 1,2});
    data.push_back(Point{ 2,4});
    data.push_back(Point{ 3,6});
    auto ret = BGIQD::LINEARFITTING::lineFit(data);
    CHECK_FLOAT(2.0f,ret.a/ret.b);
    CHECK_FLOAT(0.0f,ret.c);
    CHECK(8,ret.getY(4));
    CHECK(5,ret.getX(10));
}

TEST(MMM_121)
{
    std::vector<Point> data;
    data.push_back(Point{ 0,0+1});
    data.push_back(Point{ 1,2+1});
    data.push_back(Point{ 2,4+1});
    data.push_back(Point{ 3,6+1});
    auto ret = BGIQD::LINEARFITTING::lineFit(data);
    CHECK_FLOAT(2.0f,ret.a/ret.b);
    CHECK_FLOAT(0.5,ret.c/ret.a);
    CHECK(19,ret.getY(9));
    CHECK(4,ret.getX(9));
}
