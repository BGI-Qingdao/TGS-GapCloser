#ifndef __BIOCOMMON_SAM_BAM_SAM_PARSER_H__
#define __BIOCOMMON_SAM_BAM_SAM_PARSER_H__

#include <string>
#include <vector>

namespace BGIQD{
namespace SAM{

struct Head
{
    enum HeadType
    {
        Unknow = -1,
        HeadLine= 0 ,
        Sequence = 1 ,
        ReadGroup = 2 ,
        Program = 3,
        OneLineComment = 4
    };

    struct SequenceData
    {
        std::string name ;
        int length ;
    };

    struct VersionData
    {
        std::string version;
    };
    struct Data
    {
        SequenceData sequenceData;
        VersionData  versionData;
    };
    HeadType type ;
    Data d ;
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

struct FLAGS
{
    int ox1:1;
    int ox2:1;
    int ox4:1;
    int ox8:1;
    int ox10:1;
    int ox20:1;
    int ox40:1;
    int ox80:1;
    int ox100:1;
    int ox200:1;
    int ox400:1;
    int ox800:1;
};

union FLAGS_INT
{
    FLAGS flags;
    int num;
};


struct MatchDetail
{
    std::vector<MatchInfo> infos;
};

struct MDData
{
    int total_same;
    double IDY(int total_match) const { return double(total_same)/ double(total_match ) ; }
    bool InitFromStr(const std::string & line );
} ;

struct MatchData
{
    std::string read_name;
    FLAGS_INT flags;
    //int flag ;
    std::string ref_name;
    size_t first_match_position;
    int  quality;
    MatchDetail detail;
    int read_len;
    bool origin ;
    std::string next_ref_name ;
    int next_ref_pos;
    int insert_size ;
    bool XA;
    bool MD;
    MDData md_data;
    MatchData() : read_name("") , ref_name("")
                  , first_match_position(0)
                  , quality(0) , read_len(-1){}

    int CalcRead1Position() const;
    int CalcLeft1Position() const;
    bool IsP() const ;
    bool IsE() const ;
    bool IsPrimaryMatch() const ;
    bool IsPCRduplicae() const ;
    bool IsPEInSameRef() const ;
    bool IsPEBothMatch() const ;
    bool IsPEBothProperlyMatch() const ;
    bool IsSupplementaryMatch() const ;
    bool IsSecondaryMatch() const ;
    bool Valid() const { return ! detail.infos.empty() ; }
    bool UnMap() const ;
    bool OtherUnMap() const ;
    bool IsReverseComplete() const ;
    int firstMatchInRefNoReverse() const ;

    // the match len + insert len + delete len 
    int total_result_len() const ;

    // the match len + insert len + delete len 
    int total_match_len() const ;

    int total_in_len()  const ;

    int total_del_len() const ;

    int total_clip_len() const ;

    int total_indel_len() const ;
    //TODO : other columns . 
};// class MapData
/******************************************************************************
 *
 * Parse 1 raw sam data into struct.
 *
 * ***************************************************************************/
class LineParser
{
    public:
        LineParser( const std::string & line ) : m_line(line) {}
        bool IsVaid() const { return m_line.size() > 0 ; }
        bool IsHead() const { return m_line.at(0) == '@' ; }
        MatchData ParseAsMatchData() const ;
        Head ParseAsHead() const ;

    private:
        const std::string m_line;
    private:
        size_t ParseStringAsCIGAR(const std::string & str,size_t first_match_on_ref, MatchDetail & detail) const ;
};// class LineParse


} // namespace SAM
} // namespace BGIQD
#endif // __BIOCOMMON_SAM_BAM_SAM_PARSER_H__
