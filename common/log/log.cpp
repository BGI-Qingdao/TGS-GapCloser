#include "common/log/log.h"
#include "common/time/timetools.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace BGIQD{
namespace LOG{

    std::string detaillog::logstring(const std::string & str)
    {
        return module+"\t"
              +level+"\t"
              +BGIQD::TIME::timepoint::now().to_string() 
              +"\t:\t"+str;
    }

    /****************************************************************/

  /*  void logger::log(const char * fmt , ...) const
    {
        if( ost == NULL || theme == NULL )
            return ;
        const int buffersize = 1024 ;
        char logbuffer[buffersize];
        memset(logbuffer,0,sizeof(logbuffer));
        va_list va;
        va_start(va,fmt);
        snprintf(logbuffer,buffersize,fmt,va);
        va_end(va);
        std::string final_log = theme->logstring(logbuffer);
        (*ost)<<final_log<<std::endl;
    }*/
} //LOG
} //BGIQD
