#ifndef __COMMON_STRING_STRINGTOOLS_H__
#define __COMMON_STRING_STRINGTOOLS_H__

#include <string>
#include <vector>

namespace BGIQD{
namespace STRING{

    std::string itos( int i );


    std::string ltrim(const std::string & str);
    std::string rtrim(const std::string & str);
    std::string trim(const std::string & str);

    // split by blank
    std::vector<std::string>  split(const std::string & str) ;
    std::vector<std::string>  split(const std::string & str , const std::string & spliter ) ;
    std::vector<std::string>  split(const std::string & str , const char & spliter ) ;

    // replace all substr
    void replace_all( std::string &strBig,
            const std::string &strsrc,
            const std::string &strdst );

    bool IsNum(const std::string & str);
} //STRING
} //BGIQD



#endif //__COMMON_STRING_STRINGTOOLS_H__
