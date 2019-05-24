#include "biocommon/paf/PAF.h"

#include <sstream>
namespace BGIQD {
    namespace PAF {
        // 15      4432    0       259     +       9125    13577   9       273     242     266     0       tp:A:S  mm:i:15 gn:i:9  go:i:2  cg:Z:136M7D13M2I108M
        void PAF_Item::InitFromString(const std::string &line)
        {
            std::istringstream ist(line);
            ist>>query_name>>query_len>>query_start>>query_end>>query_char;
            ist>>target_name>>target_len>>target_start>>target_end;
            ist>>len_query_match>>len_target_match>>quality;

            std::string extra ;

            while( ! ist.eof() )
            {
                ist>>extra ;
                BGIQD::ALIGN_COMMON::ExtraInfo info ;
                info.InitFromStr(extra);
                if( info.name == "cg" )
                {
                    details.InitFromStr(info.content);
                }
                if( info.name == "MD" )
                {
                    md_data.InitFromStr(info.content);
                }
            }
        }

        PAF_Item PAF_Item::Flatten() const 
        {
            PAF_Item  ret = *this ;
            ret.query_start = 0 ;
            ret.query_end = query_len -1 ;
            if( query_char == '+' )
            {
                ret.target_start = target_start - query_start  ;
                ret.target_end = target_end + ( query_len -1 - query_end) ;
            }
            else
            {
                ret.target_start = target_start - ( query_len -1 - query_end ) ;
                ret.target_end = target_end + query_start ;
            }
            // ret.len_query_match = ?
            // ret.len_target_match = ?
            return ret ;
        }
    }
}
