#include "biocommon/fasta/fasta.h"
#include "common/files/file_reader.h"
#include "common/files/file_writer.h"
#include "common/args/argsparser.h"
#include "common/error/Error.h"

#include <sstream>
#include <fstream>
#include <cassert>

typedef BGIQD::FASTA::Id_Desc_Head  Header ;
typedef BGIQD::FASTA::Fasta<Header> Fa;
typedef BGIQD::FASTA::FastaReader<Fa> Reader;
Fa ref ;

void LoadRef( const std::string & file )
{
    std::ifstream ifs(file);
    assert( ifs.is_open() ) ;
    std::vector<Fa> refs;
    Reader::LoadAllFasta(ifs,refs);
    assert(refs.size() == 1 );
    ref = refs[0] ;
    ifs.close();
}
void printFill(std::ostream & ost , const std::string & line )
{
    static int c1 = 1 ; 
    static int c2 = 2 ;

    int scaff_id =0 , Ls=0, Le = 0 , N = 0 , Rs = 0 , Re = 0 ;
		//        >scaffold_1\tL:60001,85000\tN:2000\tR:87001,112000
    sscanf(line.c_str() , ">scaffold_%d\tL:%d,%d\tN:%d\tR:%d,%d" , &scaff_id , &Ls, &Le, &N, &Rs, &Re );

    int fill_start = Le ;
    int fill_len = N ;
    std::string fill = ref.seq.atcgs.substr(Le,N);
    ost<<'>'<<c1<<'_'<<c2<<'\n';
    ost<<fill<<'\n';
    c1+=2;
    c2+=2;
}

void ProcessEachLine(const std::string & ifile , const std::string &ofile )
{
    std::ifstream ifs(ifile);
    std::ofstream ofs(ofile);
    assert( ifs.is_open() ) ;
    assert( ofs.is_open() ) ;
    std::string line ;
    while( ! std::getline( ifs , line ).eof() )
    {
        if( line.size() < 1 ) continue ;
        if( line[0] != '>' ) continue ;
        printFill(ofs,line);
    }
    ifs.close();
    ofs.close();
}

int main(int argc , char **argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string,ref,"the input reference file");
        DEFINE_ARG_REQUIRED(std::string,fake_scaff ,"the input fake scaffold");
        DEFINE_ARG_REQUIRED(std::string,output ,"the output fill seq");
    END_PARSE_ARGS
    LoadRef(ref.to_string());
    ProcessEachLine( fake_scaff.to_string() , output.to_string() );
    return 0 ;
}
