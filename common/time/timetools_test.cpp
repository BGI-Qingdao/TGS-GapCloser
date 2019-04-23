#include "common/test/Test.h"
#include "common/time/timetools.h"
#include "common/test/Check.h"
#include <unistd.h>
#include "common/multithread/MultiThread.h"
TEST_MODULE_INIT(timetools)

using namespace BGIQD::TIME;
using namespace BGIQD::LOG;

TEST(timepoint_test)
{
    timepoint start = timepoint::now();
    sleep(2);
    timepoint end = timepoint::now();
    (test_logger)<<lstart()<<start.to_string()<<lend();
    (test_logger)<<lstart()<<end.to_string()<<lend();
    (test_logger)<<lstart()<<(end-start).to_string()<<lend();
}


TEST( multi_thread_run)
{
    timepoint start = timepoint::now();
    auto full_run = [] ()
    {
        int res = 0;
        for( int i = 0 ; i < 100000 ; i ++ )
            for( int j = 0 ; j < 10000 ; j++ )
                res +=j;
        return res;
    };

    BGIQD::MultiThread::MultiThread t_jobs;
    t_jobs.Start(3);
    t_jobs.AddJob( full_run);
    t_jobs.AddJob( full_run);
    t_jobs.AddJob( full_run);
    t_jobs.End();
    t_jobs.WaitingStop();
    timepoint end = timepoint::now();
    (test_logger)<<lstart()<<start.to_string()<<lend();
    (test_logger)<<lstart()<<end.to_string()<<lend();
    (test_logger)<<lstart()<<(end-start).to_string()<<lend();
}
