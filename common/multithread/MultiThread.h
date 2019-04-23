#ifndef __COMMON_MULTITHREAD_MULTITHREAD_H__
#define __COMMON_MULTITHREAD_MULTITHREAD_H__
    
#include "common/multithread/JobQueue.h"
#include <thread>
namespace BGIQD{
namespace MultiThread{

struct Thread 
{
    static void run(JobQueue & queue)
    {
        while(true)
        {
            auto top = queue.Get();
            if( top.first == false )
                break;
            else
            {
                top.second();
            }
        }
    }
};


struct MultiThread
{
    public:

        MultiThread() {}
        MultiThread( const MultiThread & ) = delete ;
        MultiThread& operator=(const MultiThread & ) = delete;

    public:
        void Start(int thread_num)
        {
            for( int i = 0 ; i < thread_num ; i++)
            {
                m_threads.emplace_back(new std::thread(std::bind(&Thread::run,std::ref(m_queue) )));
            }
        }

        void End()
        {
            m_queue.End();
        }

        void WaitingStop()
        {
            while(true)
            {
                size_t ends = 0;
                for(size_t i = 0 ; i<m_threads.size() ; i++)
                {
                    if( m_threads[i] )
                    {
                        m_threads[i]->join();
                        delete m_threads[i];
                        m_threads[i] = NULL ; 
                        break;
                    }
                    ends++;
                }
                if( ends == m_threads.size() )
                {
                    break;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }

        void AddJob( const Job & j ) 
        {
            m_queue.Set(j);
        }
    private:
        JobQueue m_queue ;
        std::vector<std::thread*> m_threads;
};

}
}
#endif //__COMMON_MULTITHREAD_MULTITHREAD_H__
