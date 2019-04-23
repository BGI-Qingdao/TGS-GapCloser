#ifndef __BIOCOMMON_PAF_PAF_H__
#define __BIOCOMMON_PAF_PAF_H__

#include <string>
#include "biocommon/align_common/align_result.h"

namespace BGIQD {
    namespace PAF {

        struct PAF_Item
        {
            std::string query_name;
            int query_len ;
            int query_start ;
            int query_end ;
            char query_char ;
            std::string target_name;
            int target_len ;
            int target_start ;
            int target_end ;
            int len_query_match ;
            int len_target_match ;
            int quality ;

            void InitFromString( const std::string & line );

            // force map all read length onto target
            PAF_Item Flatten() const ;

            BGIQD::ALIGN_COMMON::MatchDetail details;

            BGIQD::ALIGN_COMMON::MDData md_data;

        };
    }
}

#endif
