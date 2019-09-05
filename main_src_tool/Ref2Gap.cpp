#include "biocommon/fasta/fasta.h"
#include "common/files/file_reader.h"
#include "common/files/file_writer.h"
#include "common/args/argsparser.h"
#include "common/error/Error.h"
#include "appcommon/ScaffInfo.h"
#include "biocommon/seq/tool_func.h"

#include <sstream>
#include <fstream>
#include <cassert>
#include <map>

/********** Defines *******************************************/
typedef BGIQD::FASTA::Id_Desc_Head  Header ;
typedef BGIQD::FASTA::Fasta<Header> Fa;
typedef BGIQD::FASTA::FastaReader<Fa> Reader;
struct ContigAlign ;

/********** Global data *******************************************/

std::map<std::string , std::string> ref_seqs;
BGIQD::stLFR::ScaffInfoHelper scaff_helper;
std::map< int , ContigAlign > contig_aligns;
int max_gap_size ;

/********** Functions *******************************************/

void LoadRef( const std::string & file )
{
    std::ifstream ifs(file);
    assert( ifs.is_open() ) ;
    std::vector<Fa> refs;
    Reader::LoadAllFasta(ifs,refs);
    ifs.close();

    for(const auto & i : refs )
    {
        ref_seqs[i.head.Id]= i.seq.atcgs ;
    }
}

void LoadScaffInfos(const std::string & file )
{
    auto  in = BGIQD::FILES::FileReaderFactory
        ::GenerateReaderFromFileName(file);
    if( in == NULL )
        FATAL("failed to open xxx.updated_scaff_infos to read");

    scaff_helper.LoadAllScaff(*in);
    delete in ;
}

struct AllAligned
{
    int S1;
    int E1;
    int S2;
    int E2;
    std::string ref ;
    int contig;

    void Init( const std::string & line )
    {
        char ref_name[1024];
        for( int i = 0; i < 1024 ; i++ ) ref_name[i] = 0 ;
        sscanf(line.c_str() , "%d\t%d\t%d\t%d\t%s\t%d\t",
                &S1,
                &E1,
                &S2,
                &E2,
                &(ref_name[0]) ,
                &contig
              );
        ref=std::string(ref_name);
    }
};

struct ContigAlign
{
    std::string type ;
    std::string ref ;
    int len;
    int contig_id ;
    int ref_S;
    int ref_E;
    bool orient;

    void Init( const std::string & line , const std::vector<AllAligned> & aligns )
    {
        char buffer[120];
        for(int i = 0; i < 120 ; i++ ) buffer[i] = 0 ;
        sscanf(line.c_str(),"CONTIG\t%d\t%d%s", &contig_id , &len , &buffer[0]);
        type = std::string(buffer);
        InitSE(aligns); 
    }
    void InitSE( const std::vector<AllAligned> & aligns )
    {
        if(aligns.empty() || type != "correct" )
        {
            ref_S = 0 ;
            ref_E = 0 ;
            type = "no" ;
            return ;
        }
        if( aligns.size() ！= 1 )
        {
            ref_S = 0 ;
            ref_E = 0 ;
            type = "no" ;
            return ;
        }
        int biggest = -1 ; int biggest_index = -1 ;
        for( size_t i = 0 ; i < aligns.size() ; i++ )
        {
            int as = std::abs(aligns[i].E2 - aligns[i].S2);
            if(as > biggest )
            {
                biggest = as ;
                biggest_index = i ;
            }
        }
        assert( biggest > 0 );
        AllAligned info = aligns[biggest_index] ;
        int cs = 0 ; int ce = 0 ;
        ref = info.ref;
        if( info.S2 < info.E2 )
        {
            orient = true ;
            cs = info.S2 ;
            ce = info.E2 ;
        }
        else
        {
            orient = false ;
            cs = info.E2 ;
            ce = info.S2 ;
        }

        ref_S = info.S1 ;
        ref_E = info.E1 ;
        if ( orient )
        {
            if( cs > 1 )
                ref_S = ref_S - cs +1  ;
            if( ce < len )
                ref_E = ref_E + ( len - ce ) ;
        }
        else
        {
            if( cs > 1 )
                ref_E = ref_E + cs - 1  ;
            if( ce < len )
                ref_S = ref_S - ( len - ce ) ;
        }
    }
};

void LoadAllAligned(const std::string & file)
{
    auto  in = BGIQD::FILES::FileReaderFactory
        ::GenerateReaderFromFileName(file);
    if( in == NULL )
        FATAL("failed to open xxx.updated_scaff_infos to read");

    std::string line ;
    std::vector<AllAligned> buffer;
    while ( ! std::getline(*in, line).eof() )
    {
        if( line.empty() ) continue ;
        if( line[0] == 'C' )
        {
            ContigAlign tmp;
            tmp.Init(line,buffer);
            buffer.clear();
            contig_aligns[tmp.contig_id] = tmp;
        }
        if( std::isdigit(line[0]) )
        {
            AllAligned tmp;
            tmp.Init(line);
            buffer.push_back(tmp);
        }
        line.clear() ;
    }
    if( ! line.empty()&& line[0] == 'C' && ! buffer.empty() )
    {
        ContigAlign tmp;
        tmp.Init(line,buffer);
        buffer.clear();
        contig_aligns[tmp.contig_id] = tmp;
    }
}

void ProcessEachGap()
{
    for( auto & pair : scaff_helper.all_scaff)
    {
        auto & a_scaff = pair.second.a_scaff ;
        if( a_scaff.size() < 2 ) continue ;
        for( int i = 0 ; i < a_scaff.size() -1 ; i ++ )
        {
            auto & c1 = a_scaff[i] ;
            auto & c2 = a_scaff[i+1] ;
            // TYPE is not correct 
            if( contig_aligns.find(c1.contig_id) == contig_aligns.end() || 
                    contig_aligns.find(c2.contig_id) == contig_aligns.end() )
            {
                c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "Unaligned";
                continue ;
            }
            auto & c1_a = contig_aligns.at(c1.contig_id);
            auto & c2_a = contig_aligns.at(c2.contig_id);
            if( c1_a.type != "correct" || "correct" != c2_a.type )
            {
                c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "Uncorrect";
                continue ;
            }
            if( c1_a.ref != c2_a.ref )
            {
                c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "DiffRef";
                continue ;
            }
            assert( c1.orientation && c2.orientation );
            if( c1_a.orient != c2_a.orient )
            {
                c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "OOWrong";
                continue ;
            }
            if( c1_a.ref_S < c2_a.ref_S && c1_a.ref_E < c2_a.ref_E )
            {
                if( ! c1_a.orient )
                {
                    if( c1_a.orient != c2_a.orient )
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "OOWrong";
                        continue ;
                    }
                }
                if( c1_a.ref_E >= c2_a.ref_S )
                {
                    c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "Overlap";
                    c1.gap_size = - c1_a.ref_E + c2_a.ref_S -1 ;
                    continue ;
                }
                else
                {
                    int gap_size =  c2_a.ref_S - c1_a.ref_E -1 ;
                    if ( gap_size == 0 )
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "GAP_ZERO";
                        c1.gap_size = 0 ;
                        continue ;
                    }
                    if( gap_size < max_gap_size )
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "FILL";
                        c1.gap_size = gap_size ;
                        c1.extra[BGIQD::stLFR::ContigDetail::ONT_FILL] = ref_seqs[c1_a.ref].substr(c1_a.ref_E ,c1.gap_size);
                        continue ;
                    }
                    else
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "GAP_BIG";
                        continue ;
                    }
                }
            }
            else if ( c1_a.ref_S > c2_a.ref_S && c1_a.ref_E > c2_a.ref_E )
            {
                if(  c1_a.orient )
                {
                    if( c1_a.orient != c2_a.orient )
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "OOWrong";
                        continue ;
                    }
                }
                if( c2_a.ref_E >= c1_a.ref_S )
                {
                    c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "Overlap";
                    c1.gap_size = - c2_a.ref_E + c1_a.ref_S -1;
                    continue ;
                }
                else
                {
                    int gap_size =  c1_a.ref_S - c2_a.ref_E -1 ;
                    if ( gap_size == 0 )
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "GAP_ZERO";
                        c1.gap_size = 0 ;
                        continue ;
                    }
                    if( gap_size < max_gap_size)
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "FILL";
                        c1.gap_size = gap_size  ;
                        c1.extra[BGIQD::stLFR::ContigDetail::ONT_FILL] = 
                            BGIQD::SEQ::seqCompleteReverse( 
                            ref_seqs[c1_a.ref].substr(c2_a.ref_E,c1.gap_size)
                            ) ;
                        continue ;
                    }
                    else
                    {
                        c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "GAP_BIG";
                        continue ;
                    }
                }
            }
            else
                c1.extra[BGIQD::stLFR::ContigDetail::GAP_TYPE] = "NotLinear";

        }
    }
}

void PrintOutput(const  std::string & file)
{
    auto out = BGIQD::FILES::FileWriterFactory
        ::GenerateWriterFromFileName(file);
    if ( out == NULL )
        FATAL(" failed to open the xxx.orignal_scaff_infos to write ");

    scaff_helper.PrintAllScaff(*out);
    delete out ;
}

/********** Main Function *******************************************/
int main(int argc , char **argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string,ref,"the input reference file");
    DEFINE_ARG_REQUIRED(std::string,scaff_info ,"the input scaff_info file");
    DEFINE_ARG_REQUIRED(std::string,allaligned, "the allaligned file from quast");
    DEFINE_ARG_REQUIRED(std::string,output ,"the output scaff_info file");
    DEFINE_ARG_OPTIONAL(int , max_gap , "the max allowd gap size","1000000");
    END_PARSE_ARGS
    max_gap_size = max_gap.to_int() ;
    LoadRef(ref.to_string());
    LoadAllAligned( allaligned.to_string());
    LoadScaffInfos(scaff_info.to_string());
    ProcessEachGap();
    PrintOutput(output.to_string());
    return 0 ;
}
