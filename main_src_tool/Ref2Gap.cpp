#include "biocommon/fasta/fasta.h"
#include "common/files/file_reader.h"
#include "common/files/file_writer.h"
#include "common/args/argsparser.h"
#include "common/error/Error.h"

#include <sstream>
#include <fstream>
#include <cassert>
#include <map>

typedef BGIQD::FASTA::Id_Desc_Head  Header ;
typedef BGIQD::FASTA::Fasta<Header> Fa;
typedef BGIQD::FASTA::FastaReader<Fa> Reader;
Fa ref ;

std::map<std::string , std::string> ref_seqs;

void LoadRef( const std::string & file )
{
    std::ifstream ifs(file);
    assert( ifs.is_open() ) ;
    std::vector<Fa> refs;
    Reader::LoadAllFasta(ifs,refs);
    ifs.close();

    for(const auto & i : refs )
    {
        ref_seqs[i.head.Head()]= i.seqs.atcgs ;
    }
}

BGIQD::stLFR::ScaffInfoHelper scaff_helper;

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
    float IDY;

    void Init( const std::string & line )
    {
        char ref_name[1024];
        for( int i = 0; i < 1024 ; i++ ) ref_name = 0 ;
        sscanf(line.c_str() , "%d\t%d\t%d\t%d\t%s\t%d\t%f\t%s",
                &S1,
                &E1，
                &S2,
                &S2,
                &ref_name[0]
                &contig,
                &IDY
        ref=std::string(ref_name);
    }
};

struct ContigAlign
{
    char type[120];
    std::string ref ;
    int len;
    int contig_id ;
    int ref_S;
    int ref_E;
    bool orient;

    void Init( const std::string & line , const std::vector<AllAligned> & aligns )
    {
        for(int i = 0; i < 120 ; i++ ) type[i] = 0 ;
        sscanf(line.c_str(),"CONTIG\t%d\t%d%s", &contig_id , &len , &type[0]);
        InitSE(aligns); 
    }
    void InitSE( const std::vector<AllAligned> & aligns )
    {
        if(aligns.empty() || strcmp("correct",type ) != 0 )
        {
            ref_S = 0 ;
            ref_E = 0 ;
            return ;
        }
        int biggest = -1 ; int biggest_index = -1 ;
        for( size i = 0 ; i < aligns.size() ; i++ )
        {
            int as = std::abs(aligns[i].E2 - aligns[i].S2);
            if(as > biggest )
            {
                biggest = as ;
                biggest_index = i ;
            }
        }
        assert( biggest > 0 );
        AllAligned info = aligns[i] ;
        int cs = 0 ; int ce = 0 ;
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

        if( cs > 1 )
            ref_S = ref_s - cs +1  ;
        if( ce < len )
            ref_E = ref_e + ( len - cs ) ;
    }
};

std::map< int , ContigAlign > contig_aligns;
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
            contig_aligns[tmp.contig_id] = tmp;.
        }
        if( std::isdigit(line[0]) )
        {
           AllAligned tmp;
           tmp.Init(line);
           buffer.push_back(line);
        }
        line.clear() ;
    }
    if( ! line.empty()&& line[0] == 'C' && ! buffer.empty() )
    {
        ContigAlign tmp;
        tmp.Init(line,buffer);
        buffer.clear();
        contig_aligns[tmp.contig_id] = tmp;.
    }
}

void ProcessEachGap()
{

}

void PrintOutput(const  std::string & file)
{

}

int main(int argc , char **argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string,ref,"the input reference file");
        DEFINE_ARG_REQUIRED(std::string,scaff_info ,"the input scaff_info file");
        DEFINE_ARG_REQUIRED(std::string,allaligned, "the allaligned file from quast")
        DEFINE_ARG_REQUIRED(std::string,output ,"the output scaff_info file");
    END_PARSE_ARGS

    LoadRef(ref.to_string());
    LoadAllAligned( allaligned.to_string());
    LoadScaffInfos(scaff_info.to_string());
    ProcessEachGap();
    PrintOutput(output.to_string());
    return 0 ;
}
