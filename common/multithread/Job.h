#ifndef __COMMON_MULTITHREAD_JOB_H__
#define __COMMON_MULTITHREAD_JOB_H__

#include <functional>
namespace BGIQD{
    namespace MultiThread{
        typedef std::function<void(void)> Job;
    }
}

#endif //__COMMON_MULTITHREAD_JOB_H__

