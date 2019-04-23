#ifndef __BIOCOMMON_PAIR_PAIR_SAM_PARSER_H__
#define __BIOCOMMON_PAIR_PAIR_SAM_PARSER_H__

#include "biocommon/sam_bam/sam_parser.h"
#include <istream>
#include <vector>
#include <map>
namespace BGIQD{
namespace SAM{

class PairedSAMParser
{
    public:
        PairedSAMParser( std::istream & ist ) : m_ist(ist) {}

        //
        // Get a pair of data from sam file.
        // If no valid pair , return data are invalid.
        //
        // Make sure : 
        //  1. reads of 1 pair have same read name 
        //  2. reads of 1 pair are one are one in sam file .
        //
        std::pair<MatchData , MatchData> CurrentPair();

        bool eof() const { return m_ist.eof() ; }

    private:
        std::vector<MatchData> m_cache;
        std::vector<MatchData> m_cache_next;
        std::istream & m_ist;
};// class PairedSAMParser

}// namespace SAM
}// namespace BGIQD
#endif //__BIOCOMMON_PAIR_PAIR_SAM_PARSER_H__
