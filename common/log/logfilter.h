#ifndef __COMMON_LOG_LOGFILTER_H__
#define __COMMON_LOG_LOGFILTER_H__

#include <string>
#include "log.h"
#include <vector>

namespace BGIQD{
namespace LOG{

/*****************************************************
 * how to define specfic log
 *  
 *  * define common log level
 *      -l "module:E/I/W/D:file"
 *      default is all:I:cerr
 *
 *  * define special log detail for special module
 *      -L "module:E/I/W/D:on/off:file"
 *
 * ***************************************************/
enum loglevel :int
{
    ERROR = 0,
    INFO = 1,
    WARNING =2 ,
    DEBUG = 3
};

class logfilter
{
    public:
        static logfilter & singleton();
        //
        // parse a config
        // do this before get any logger
        void parse(const std::string & arg);

        // 
        // get a logger based on config.
        // it is user's duty to release this logger
        void get(const std::string module,loglevel level, logger & ret);
    private:

        struct modulelogrole
        {
            std::string module;
            loglevel maxlevel;
            std::string filename;
        };

        struct specialrole
        {
            std::string module;
            loglevel maxlevel;
            bool on;
            std::string filename;
        };
        static logfilter * the_one ;
        logfilter(){}
        ~logfilter();
        logfilter( const logfilter & ) ;
        logfilter operator=( const logfilter & );
        std::vector<specialrole> roles;
        std::vector<ilogtheme *>themes;

        static std::string levelname(loglevel l)
        {
            if( l == loglevel::ERROR)
                return "ERROR";
            if( l == loglevel::INFO)
                return "INFO";
            if( l == loglevel::DEBUG)
                return "DEBUG";
            if( l == loglevel::WARNING)
                return "WARNING";
            return "UNKNOW";
        }
};

}//LOG
}//BGIQD

#endif //__COMMON_LOG_LOGFILTER_H__
