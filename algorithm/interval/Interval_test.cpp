#include "algorithm/interval/Interval.h"
#include "common/test/Test.h"

TEST_MODULE_INIT(Interval)

typedef BGIQD::INTERVAL::Interval<int
    ,BGIQD::INTERVAL::IntervalType::Left_Close_Right_Close> IntervalInt01;

typedef BGIQD::INTERVAL::Interval<int
    ,BGIQD::INTERVAL::IntervalType::Left_Close_Right_Open> IntervalInt02;

typedef BGIQD::INTERVAL::Interval<int
    ,BGIQD::INTERVAL::IntervalType::Left_Open_Right_Close> IntervalInt03;

typedef BGIQD::INTERVAL::Interval<int
    ,BGIQD::INTERVAL::IntervalType::Left_Open_Right_Open> IntervalInt04;

TEST(TIntervalInt01)
{
    IntervalInt01 t(1,5);
    CHECK(true , t.IsContain(1));
    CHECK(true , t.IsContain(5));
    CHECK(true , t.IsContain(3));
    CHECK(false, t.IsContain(0));
    CHECK(false, t.IsContain(6));

    IntervalInt01 t1(1,4);
    CHECK(true , t.IsContain(t1));
    CHECK(false, t1.IsContain(t));
    IntervalInt01 t2(1,7);
    CHECK(1, t.Overlap(t2).min);
    CHECK(5, t.Overlap(t2).max);
}
TEST(TIntervalInt02)
{
    IntervalInt02 t(1,5);
    CHECK(true , t.IsContain(1));
    CHECK(false, t.IsContain(5));
    CHECK(true , t.IsContain(3));
    CHECK(false, t.IsContain(0));
    CHECK(false, t.IsContain(6));
    IntervalInt02 t1(1,4);
    CHECK(true , t.IsContain(t1));
    CHECK(false, t1.IsContain(t));
    IntervalInt02 t2(1,7);
    CHECK(1, t.Overlap(t2).min);
    CHECK(5, t.Overlap(t2).max);
}

TEST(TIntervalInt03)
{
    IntervalInt03 t(1,5);
    CHECK(false, t.IsContain(1));
    CHECK(true , t.IsContain(5));
    CHECK(true , t.IsContain(3));
    CHECK(false, t.IsContain(0));
    CHECK(false, t.IsContain(6));
    IntervalInt03 t1(1,4);
    CHECK(true , t.IsContain(t1));
    CHECK(false, t1.IsContain(t));
    IntervalInt03 t2(1,7);
    CHECK(1, t.Overlap(t2).min);
    CHECK(5, t.Overlap(t2).max);
}


TEST(TIntervalInt04)
{
    IntervalInt04 t(1,5);
    CHECK(false, t.IsContain(1));
    CHECK(false, t.IsContain(5));
    CHECK(true , t.IsContain(3));
    CHECK(false, t.IsContain(0));
    CHECK(false, t.IsContain(6));
    IntervalInt04 t1(1,4);
    CHECK(true , t.IsContain(t1));
    CHECK(false, t1.IsContain(t));
    IntervalInt04 t2(1,7);
    CHECK(1, t.Overlap(t2).min);
    CHECK(5, t.Overlap(t2).max);
}
