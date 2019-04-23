#include "biocommon/align_common/align_result.h"
#include "common/string/stringtools.h"
#include <cassert>

namespace BGIQD {
    namespace ALIGN_COMMON {

        int MatchDetail::total_del_len()  const 
        {
            int ret = 0 ;
            for ( const  auto & x : infos )
            {
                if ( x.type == CIGAR::D 
                  )
                {
                    ret += x.len;
                }
            }
            return ret ;
        }

        int MatchDetail::total_in_len()  const 
        {
            int ret = 0 ;
            for ( const  auto & x : infos )
            {
                if ( x.type == CIGAR::I 
                  )
                {
                    ret += x.len;
                }
            }
            return ret ;
        }


        int MatchDetail::total_clip_len()  const 
        {
            int ret = 0 ;
            for ( const  auto & x : infos )
            {
                if ( x.type == CIGAR::S
                    || x.type == CIGAR::H 
                  )
                {
                    ret += x.len;
                }
            }
            return ret ;
        }
        int MatchDetail::total_indel_len()  const 
        {
            int ret = 0 ;
            for ( const  auto & x : infos )
            {
                if ( x.type == CIGAR::I 
                || x.type == CIGAR::D 
                   )
                {
                    ret += x.len;
                }
            }
            return ret ;
        }

        int MatchDetail::total_result_len() const
        {
            int ret = 0 ;
            for ( const  auto & x : infos )
            {
                if ( x.type == CIGAR::M 
                    || x.type == CIGAR::X
                    || x.type == CIGAR::EQUAL
                    || x.type == CIGAR::D 
                    || x.type == CIGAR::I 
                  )
                {
                    ret += x.len;
                }
            }
            return ret ;
        }

        int MatchDetail::total_match_len() const
        {
            int ret = 0 ;
            for ( const  auto & x : infos )
            {
                if ( x.type == CIGAR::M 
                    || x.type == CIGAR::X
                    || x.type == CIGAR::EQUAL
                  )
                {
                    ret += x.len;
                }
            }
            return ret ;
        }




        bool MDData::InitFromStr( const std::string &line )
        {
            total_same = 0 ;
            std::string int_value;
            for ( int i = 0 ; i < (int)line.size() ; i++ )
            {
                char c = line[i] ;
                if( std::isdigit( c ) )
                {
                    int_value.push_back( c) ;
                }
                else
                {
                    if( ! int_value.empty() )
                    {
                        total_same += std::stoi(int_value);
                        int_value.clear();
                    }
                }
            }
            if( ! int_value.empty() )
            {
                total_same += std::stoi(int_value);
                int_value.clear();
            }
            return total_same > 0 ;
        }

        void ExtraInfo::InitFromStr( const std::string & line )
        {
            auto items = BGIQD::STRING::split(line,":");
            assert(items.size() == 3 );
            name = items[0] ;
            assert(name.size() == 2 );
            assert(items[1].size() == 1 );
            type = items[1][0] ;
            content =  items[2] ;
        }

        int MatchDetail::InitFromStr( const std::string &str, int first_match_on_ref )
        {
            std::string  number_buffer;
            MatchInfo info_buffer;
            size_t curr_position_on_ref = first_match_on_ref;
            size_t curr_position_on_read = 0 ;
            size_t len = str.size() ;
            size_t ret = 0;
            auto append_new_info = [ &info_buffer 
                , &curr_position_on_ref , &curr_position_on_read
                , this  , & ret ]
                ( CIGAR type , int read_move  , int ref_move)
                {
                    info_buffer.type = type ;
                    info_buffer.start_position_on_read = -1 ;
                    info_buffer.end_position_on_read = -1 ;
                    info_buffer.start_position_on_ref = -1 ;
                    info_buffer.end_position_on_ref = -1 ;
                    info_buffer.len = 0 ;
                    if ( read_move > 0 )
                    {
                        info_buffer.start_position_on_read = curr_position_on_read;
                        info_buffer.end_position_on_read = curr_position_on_read + read_move -1 ;
                        ret = info_buffer.end_position_on_read + 1;
                        curr_position_on_read += read_move ;
                        info_buffer.len =  read_move ;
                    }
                    if ( ref_move > 0 )
                    {
                        info_buffer.start_position_on_ref = curr_position_on_ref ;
                        info_buffer.end_position_on_ref = curr_position_on_ref + ref_move -1 ;
                        curr_position_on_ref += ref_move ;
                        info_buffer.len =  ref_move;
                    }

                    infos.push_back(info_buffer);
                };

            for(size_t i = 0 ; i < len ; i++ )
            {
                switch(str[i])
                {
                    case '0'...'9':
                        number_buffer+=str[i];
                        break;
                    case '*':
                        infos.clear();
                        append_new_info( CIGAR::NONE , -1 , -1 );
                        return ret;
                    case 'M':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::M, length , length );
                            number_buffer.clear();
                        }
                        break;
                    case '=':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::EQUAL, length , length );
                            number_buffer.clear();
                        }
                        break;
                    case 'X':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::X, length , length );
                            number_buffer.clear();
                        }
                        break;
                    case 'I':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::I, length , -1);
                            number_buffer.clear();
                        }
                        break;
                    case 'H':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::H, length , -1);
                            number_buffer.clear();
                        }
                        break;
                    case 'S':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::S, length , -1);
                            number_buffer.clear();
                        }
                        break;
                    case 'D':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::D, -1, length );
                            number_buffer.clear();
                        }
                        break;
                    case 'P':
                        //TODO :
                        number_buffer.clear();
                        break;
                    case 'N':
                        {
                            int length = std::stoi(number_buffer);
                            append_new_info( CIGAR::N, -1 , length );
                            number_buffer.clear();
                        }
                        break;
                }
            }
            return ret;
        }
    }
}
