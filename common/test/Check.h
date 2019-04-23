#ifndef __LFR_UTILS_CHECK_H__
#define __LFR_UTILS_CHECK_H__

#define CHECK_STRUCT( expect , value )\
    if( expect != value ) \
        std::cerr<<__FILE__<<":"<<__LINE__<<"struct not match !!"<<std::endl; 

#define CHECK_STRUCT_AND_ONERR( expect , value , action) \
    if( expect != value ) \
    {\
        std::cerr<<__FILE__<<":"<<__LINE__<<"struct not match !!"<<std::endl;\
        action \
    }

#define CHECK_FLOAT( expect , value ) \
    if( ! (expect - value <= 0.000001 || value - expect <= 0.000001) )  \
        std::cerr<<__FILE__<<":"<<__LINE__<<" expect "<<expect<<" but "<<value<<std::endl; 



#define CHECK( expect , value ) \
    if( expect != value ) \
        std::cerr<<__FILE__<<":"<<__LINE__<<" expect "<<expect<<" but "<<value<<std::endl; 

#define CHECK_AND_ONERR( expect , value , action) \
    if( expect != value ) \
    {\
        std::cerr<<__FILE__<<":"<<__LINE__<<" expect "<<expect<<" but "<<value<<std::endl;\
        action \
    }


#endif //__LFR_UTILS_CHECK_H__
