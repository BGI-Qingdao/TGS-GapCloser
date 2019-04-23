#ifndef __COMMON_ERROR_H__
#define __COMMON_ERROR_H__

#include <cassert>
#include <iostream>

#define FATAL( msg ) \
    {\
        assert( 0 && msg );\
        std::cerr<<"FATAL : "<<msg<<std::endl;\
        std::cerr<<"now exiting program ... "<<msg<<std::endl;\
        exit(0);\
    }

#define WARN( msg ) \
    {\
        std::cerr<<"WARN: "<<msg<<std::endl;\
    }
#endif //__COMMON_ERROR_H__
