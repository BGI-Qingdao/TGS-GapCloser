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
                if( *i == 'A')
                    ret[index++] = 'T';
                if( *i == 'G')
                    ret[index++] = 'C';
                if( *i == 'C')
                    ret[index++] = 'G';
                if( *i == 'T')
                    ret[index++] = 'A';
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
    }
}
#endif// __BIOCOMMON_SEQ_TOOL_FUNC_H__
