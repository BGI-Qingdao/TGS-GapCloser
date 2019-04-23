#include "stringtools.h"
#include <sstream>

namespace BGIQD{
namespace STRING{

    std::string itos( int i )
    {
        std::ostringstream ost;
        ost<<i;
        return ost.str();
    }

    std::vector<std::string>  split( const std::string & str , const std::string & spliter ) 
    {
        std::vector<std::string> ret;
        std::string::size_type pos1 = 0;
        std::string::size_type pos2 = str.find(spliter);
        while( pos2 != str.npos )
        {
            auto item = str.substr( pos1 , pos2-pos1 ) ;
            if( ! item.empty() )
                ret.push_back( item );
            pos1 = pos2 + spliter.length() ;
            pos2 = str.find( spliter , pos1 ) ;
        }
        if( pos1 != str.length() )
        {
            ret.push_back(str.substr(pos1));
        }
        return ret ;
    }

    std::vector<std::string>  split( const std::string & str , const char& spliter ) 
    {
        std::vector<std::string> ret;
        std::string::size_type pos1 = 0;
        std::string::size_type pos2 = str.find(spliter);
        while( pos2 != str.npos )
        {
            auto item = str.substr( pos1 , pos2-pos1 ) ;
            if( ! item.empty() )
                ret.push_back( item );
            pos1 = pos2 + 1 ;
            pos2 = str.find( spliter , pos1 ) ;
        }
        if( pos1 != str.length() )
        {
            ret.push_back(str.substr(pos1));
        }
        return ret ;
    }
    std::string ltrim(const std::string & str)
    {
        return str.substr(str.find_first_not_of(" \n\t\r"));
    }
    std::string rtrim(const std::string & str)
    {
        return str.substr(0,str.find_last_not_of(" \n\r\t")+1);
    }
    std::string trim(const std::string & str)
    {
        return ltrim(rtrim(str));
    }

    std::vector<std::string>  split( const std::string & str) 
    {
        std::vector<std::string> ret;
        bool nb = false ;
        size_t nb_index = 0;
        for( size_t i = 0 ; i<str.size() ; i++ )
        {
            if( str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t' )
            {
                if(nb)
                {
                    ret.push_back(str.substr(nb_index,i-nb_index));
                    nb = false ;
                }
            }
            else
            {
                if( !nb )
                {
                    nb =true ;
                    nb_index = i ;
                }
            }
        }
        if(nb)
        {
            ret.push_back(str.substr(nb_index,str.size() - nb_index));
            nb = false ;
        }
        return ret ;
    }

    void replace_all( std::string &strBig,
            const std::string &strsrc,
            const std::string &strdst )
    {
        std::string::size_type pos = 0;
        std::string::size_type srclen = strsrc.size();
        std::string::size_type dstlen = strdst.size();

        while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
        {
            strBig.replace( pos, srclen, strdst );
            pos += dstlen;
        }
    }
    bool IsNum(const std::string & str)
    {
        for( char i : str )
        {
            if( ! std::isdigit(i) )
                return false ;
        }
        return true ;
    }
}
}
