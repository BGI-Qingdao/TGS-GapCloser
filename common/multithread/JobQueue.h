#ifndef __COMMON_MULTITHREAD_JOBQUEUE_H__
#define __COMMON_MULTITHREAD_JOBQUEUE_H__

#include <queue>
#include <condition_variable>
#include <mutex>
#include "common/multithread/Job.h"
#include <iostream>
namespace BGIQD{
namespace MultiThread{

    struct JobQueue
    {
        public:
            JobQueue() : end( false ) {}
            JobQueue( const  JobQueue &) = delete;
            JobQueue & operator=(const JobQueue & ) =delete ;
        public:
            void Set( const Job & j)
            {
                std::lock_guard<std::mutex> lk(m_mutex);
                m_queue.push(j);
                if( m_queue.size() == 1 )
                    m_condition_variable.notify_one() ;
            }

            std::pair<bool , Job> Get()
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                while( m_queue.empty() )
                {
                    if( end )
                        return std::make_pair(false , Job());
                    m_condition_variable.wait(lk);
                    continue ;
                }
                Job j =  m_queue.front();
                m_queue.pop();
                lk.unlock();
                return std::make_pair(true,j);
            }
            void End()
            {
                {
                    std::lock_guard<std::mutex> lk(m_mutex);
                    end = true ;
                }
                m_condition_variable.notify_all();
            }
        private:
            std::mutex m_mutex;
            std::condition_variable m_condition_variable;
            std::queue<Job > m_queue;
            bool end;
    };

}
}
#endif //__COMMON_MULTITHREAD_JOBQUEUE_H__
