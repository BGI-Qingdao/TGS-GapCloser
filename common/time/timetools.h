#ifndef __COMMON_TIME_TIMETOOL_H__
#define __COMMON_TIME_TIMETOOL_H__

#include <string>
#include <sys/time.h>

namespace BGIQD{
namespace TIME{

struct timeperoid;

struct timepoint
{
    public:

        timeval wall;
        clock_t cpu;

    public:

        static timepoint now() 
        {
            timepoint ret ;
            gettimeofday( &ret.wall ,NULL);
            ret.cpu = clock();
            return ret;
        }

        timeperoid operator-(const timepoint & prev_point ) const ;
        timepoint operator-(const timeperoid & back_period ) const ;
        timepoint operator+(const timeperoid & next_period ) const ;

        bool operator < ( const timepoint & other ) const ;
        bool operator == ( const timepoint & other ) const ;

    public:
        std::string to_string() const ;
        /*
        {
        }*/
};

struct timeperoid 
{
    public:
        timeval wall;
        clock_t cpu;
    public:
        timeperoid operator-( const timeperoid & other ) const ;
        timeperoid operator+( const timeperoid & other ) const ;

        bool operator < ( const timeperoid& other ) const ;
        bool operator == ( const timeperoid& other ) const ;

    public:
        std::string to_string() const ;
};

} //namespace BGIQD
} //namespace TIME
#endif //__COMMON_TIME_TIMETOOL_H__
