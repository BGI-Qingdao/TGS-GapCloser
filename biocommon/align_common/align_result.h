#ifndef __BIOCOMMON_ALIGN_COMMON_H__
#define __BIOCOMMON_ALIGN_COMMON_H__

#include <vector>
#include <string>

namespace BGIQD {
    namespace ALIGN_COMMON {

        struct ExtraInfo
        {
            std::string name ;
            char type ;
            std::string content ;

            void InitFromStr( const std::string & str );
        };

        enum CIGAR
        {
            NONE = -1,
            M = 0 ,
            I = 1 ,
            D = 2 ,
            N = 3 ,
            S = 4 ,
            H = 5 ,
            P = 6 ,
            EQUAL = 7 ,
            X = 8 ,
        };

        struct MatchInfo
        {
            CIGAR type;
            int start_position_on_read;  // this index start from 0
            int end_position_on_read;
            int start_position_on_ref;   // this index start from 1
            int end_position_on_ref;
            int len ;
            // start and end construct a [ start , end ] area .
        };

        struct MatchDetail
        {
            std::vector<MatchInfo> infos;
            int InitFromStr( const std::string & line , int first_match_on_ref = 0 );

            // the match len + insert len + delete len 
            int total_result_len() const ;

            // the match len + insert len + delete len 
            int total_match_len() const ;

            int total_in_len()  const ;

            int total_del_len() const ;

            int total_clip_len() const ;

            int total_indel_len() const ;
        };

        struct MDData
        {
            int total_same;
            bool InitFromStr(const std::string & line );
        } ;

    }
}

#endif
