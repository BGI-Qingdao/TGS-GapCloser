#ifndef __COMMON_LOG_LOG_H__
#define __COMMON_LOG_LOG_H__

#include <string>
#include <ostream>
#include <sstream>
#include "common/time/timetools.h"

namespace BGIQD{
namespace LOG{

class ilogtheme
{
    public:
        virtual std::string logstring(const std::string & str) = 0 ;
        virtual ~ilogtheme() {}
};

class detaillog
{
    public:
        virtual std::string logstring(const std::string & str) final ;
        virtual ~detaillog() {}
        detaillog( const std::string m, const std::string l)
            : module(m)
            , level(l) {}
    private:
        std::string module;
        std::string level;
};

class simplelog
{
    public:
        virtual std::string logstring(const std::string & str) final
        {
            return std::string(">\t")+str;
        }
        virtual ~simplelog() {}

};

class logfilter ;

struct lstart {};
struct lend{};

class logger
{
    public:
        // log some staff , but how and where to write those staff
        // depends on what theme and ost is .
  //      void log( const char * fmt , ... ) const ;

        template< class T >
        logger & operator << (const T & t)
        {
            buffer<<t;
            return *this;
        }

        logger & operator << (const lstart & )
        {
            buffer.str("");
            return *this;
        }

        logger & operator << (const lend & )
        {
            if( ost != NULL && theme != NULL )
            {
                (*ost)<<theme->logstring(buffer.str())
                      <<std::endl;
            }
            buffer.str("");
            return *this;
        }

        friend class logfilter;
    private:
        // NOTICE : logge doesn't free ost1 or theme1 .
        //          it's user's duty to manager their life cycle.
        std::ostringstream buffer;
        std::ostream * ost;
        ilogtheme * theme;
};

class timer
{
    public:
        timer( logger & a_logger , const std::string &job_description)
            : start(TIME::timepoint::now())
            , l(a_logger)
            , jobdec(job_description) {
                (l)<<lstart()<<jobdec<< " start now ... "<<lend();
            }

        ~timer()
        {
            TIME::timepoint end = TIME::timepoint::now();
                std::string last = (end-start).to_string();
                (l)<<lstart()<<jobdec<< " finish. used "<<last<<lend();
        }
    private:
        TIME::timepoint start ;
        logger & l;
        std::string jobdec;
};

}//LOG
}//BGIQD

#endif //__COMMON_LOG_LOG_H__
