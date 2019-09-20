#ifndef __BIOCOMMON_SEQ_TOOL_FUNC_H__
#define __BIOCOMMON_SEQ_TOOL_FUNC_H__

#include <string>
#include <sstream>

namespace BGIQD{
    namespace SEQ{

        static std::string blockSeq(const std::string & atcgs , int weight )
        {
            if( weight < 1 )
                return atcgs ;
            else
            {
                std::ostringstream ost;
                int i = 1 ;
                for( char c : atcgs)
                {
                    ost<<c;
                    if( i % weight == 0 || i == (int)atcgs.size() )
                        ost<<'\n';
                    i++ ;
                }
                return ost.str();
            }
        }

        static std::string seqCompleteReverse(const std::string & line)
        {
            std::string ret ;
            ret.resize(line.size(),'N');
            int index = 0;
            for( auto i = line.rbegin() ; i!= line.rend() ; i++)
            {
                if( *i == 'A' || *i == 'a' )
                    ret[index++] = 'T';
                else if( *i == 'G' || *i == 'g' )
                    ret[index++] = 'C';
                else if( *i == 'C' || *i == 'c' )
                    ret[index++] = 'G';
                else if( *i == 'T'|| *i == 't' )
                    ret[index++] = 'A';
                else 
                    ret[index++] = *i ;
            }
            return ret;
        }

        static bool isSeqPalindrome(const std::string & line)
        {
            if( line.size() % 2  == 1 )
                return false;
            int len = line.size();
            for( int i = 0 ; i < len /2 ; i++ )
            {
                if( line[i] == 'A' && line[len-i-1] != 'T' )
                    return false ;
                if( line[i] == 'T' && line[len-i-1] != 'A' )
                    return false ;
                if( line[i] == 'G' && line[len-i-1] != 'C' )
                    return false ;
                if( line[i] == 'C' && line[len-i-1] != 'G' )
                    return false ;
            }
            return true;
        }
        static bool isValid( const std::string & line )
        {
            for(const auto & c : line )
            {
                if( c == 'a'||c=='A')
                    continue ;

                if( c == 'g'||c=='G')
                    continue ;

                if( c == 't'||c=='T')
                    continue ;

                if( c == 'c'||c=='C')
                    continue ;

                if( c == 'n'||c=='N')
                    continue ;

                return false ;
            }
            return true ;
        }

        static bool onlyN( const std::string & line )
        {

            for(const auto & c : line )
            {
                if( c != 'n' && c!='N')
                    return false ;
            }
            return true ;
        }

        static bool hasN( const std::string & line )
        {

            for(const auto & c : line )
            {
                if( c == 'n'||c=='N')
                    return true ;
            }
            return false ;
        }
    }
}
#endif// __BIOCOMMON_SEQ_TOOL_FUNC_H__
