#include "biocommon/fasta/fasta.h"

#include "common/files/file_reader.h"
#include "common/files/file_writer.h"
#include "common/args/argsparser.h"
#include "common/error/Error.h"

#include <sstream>
#include <string>
#include <cstdlib>

typedef BGIQD::FASTA::Id_Desc_Head  Header ;
typedef BGIQD::FASTA::Fasta<Header> Fa;

struct RefContig
{
    int start_pos ; /* 1 base */
    int end_pos ;   /* 1 base */

    void Init() 
    {
        start_pos = 0 ;
        end_pos = 0 ;
    }
    bool Valid() const 
    {
        return end_pos > start_pos && start_pos > 0 ;
    }
    bool Empty() const 
    {
        return start_pos == 0 && end_pos == 0 ;
    }
};

struct SimScaff
{
    private:
        int lstart ;  /* 1 base */
        int lend ;    /* 1 base */
        int rstart ;  /* 1 base */
        int rend ;    /* 1 base */

        void Init() 
        {
            lstart = 0 ;
            lend = 0 ;
            rstart = 0 ;
            rend = 0;
        }

        int nlen() const { return rstart-lend-1; }

    public:

        bool Valid()  const
        {
            return rend > rstart 
                && rstart > lend+1  
                && lend >lstart
                && lstart > 0 ;
        }

        Fa ToReal( const Fa & ref , int index ) const 
        {
            assert(nlen() > 0 );
            Fa ret ;
            std::ostringstream ost;
            ost <<">scaffold_"<<index
                <<"\tL:"<<lstart<<','<<lend
                <<"\tN:"<<nlen()
                <<"\tR:"<<rstart<<','<<rend
                ;
            ret.AddHead( ost.str() );
            ret.AddSeq(ref.seq.atcgs.substr(lstart-1,lend-lstart+1));
            ret.AddSeq(std::string( nlen() , 'N' ) );
            ret.AddSeq(ref.seq.atcgs.substr(rstart-1 , rend-rstart + 1 ));
            return ret ;
        }

        static SimScaff GenSimScaff( 
                const RefContig & ref_contig ,
                int start_pos ,
                int & next_start_pos ,
                int left_len ,
                int right_len ,
                int n_len_max 
                )
        {

            int need_space = left_len + right_len + n_len_max ;
            SimScaff ret ;
            ret.Init() ;
            if( ref_contig.end_pos - start_pos +1 < need_space  )
                return ret ;
            ret.lstart = start_pos ;
            ret.lend = start_pos + left_len -1 ;
            ret.rstart = ret.lend + 1 + n_len_max ;
            ret.rend = ret.rstart + right_len -1 ;
            next_start_pos = ret.rend +1 ;
            return ret ;
        }
};

int random_contig(int seed)
{
    if(seed != 0 )
        return seed ;
    else
        return rand() % 29000 + 1000 ;
}

int random_gap(int seed)
{
    if(seed != 0 )
        return seed ;
    else
        return rand() % 10000 +1;
}
int random_start(int seed)
{
    if(seed != 0 )
        return seed ;
    else
        return rand() % 1000 ;
}
int main(int argc , char **argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string,input,"the input reference file");
        DEFINE_ARG_REQUIRED(std::string,output,"the output scaffold file");
        DEFINE_ARG_OPTIONAL(int , contig_len , "the length of contig , 0 means random from [1K~30K]","0");
        DEFINE_ARG_OPTIONAL(int , gap_len , "the length of gap , 0 means random from [1~10K]","0");
        DEFINE_ARG_OPTIONAL(int , start_scaffold, "the shift size for 1st scaffold, 0 means random from [0,1K]" ,"0");
    END_PARSE_ARGS

    if( contig_len.to_int() < 0 )
        FATAL(" ERROR : contig_len < 0 !! ");
    if( start_scaffold.to_int() < 0)
        FATAL(" ERROR : contig_len < 0 !! ");
    srand(time(0));
    // Load ref 
    std::vector<Fa> AllRef;
    BGIQD::FASTA::FastaReader<Fa> Reader ;
    auto rin = BGIQD::FILES::FileReaderFactory
        ::GenerateReaderFromFileName(input.to_string());
    if( !rin)
        FATAL( " ERROR : failed to open input file to read !!!" );
    Reader.LoadAllFasta(*rin ,AllRef); 
    delete rin ;
    if( AllRef.size() < 1 )
        FATAL("no ref sequence loaded !!! error !!! exit ... ")

    if( AllRef.size() > 1 )
    {
        std::cerr<<"more than 1 ref sequence loaded !!! error !!! exit ... "<<std::endl;
        std::cerr<<"this tool only suppert 1 ref sequence!!!"<<std::endl;
        return -1 ;
    }
    // Split ref by N
    const auto & the_ref =  AllRef[0] ;
    const auto &seq = the_ref.seq.atcgs ;
    bool n_prev = false ;
    int curr_pos = 0 ;
    std::vector<RefContig> AllRefContig;
    RefContig tmp ;
    tmp.Init() ;
    for( int i = 0 ; i < (int)seq.size() ; i ++ )
    {
        curr_pos ++ ;
        char x = seq.at(i);
        if( x == 'n' || x == 'N' )
        {
            if( ! n_prev )
            {
                if( tmp.Valid() )
                {
                    AllRefContig.push_back(tmp);
                    tmp.Init();
                }
                n_prev = true ;
            }
        }
        else
        {
            if( n_prev )
            {
                assert( tmp.Empty() );
                tmp.start_pos = curr_pos ;
                n_prev = false ;
            }
            else
                tmp.end_pos = curr_pos ;
        }
    }
    if( tmp.Valid() )
        AllRefContig.push_back(tmp);

    // Gen AllScaff
    std::vector<SimScaff> scaffs;
    for( const auto & a_ref_contig : AllRefContig )
    {
        int start_pos = a_ref_contig.start_pos + random_start(start_scaffold.to_int());
        while(1)
        {

            auto a_scaff = SimScaff::GenSimScaff(a_ref_contig,
                    start_pos
                    ,start_pos
                    ,random_contig(contig_len.to_int())
                    ,random_contig(contig_len.to_int())
                    ,random_gap(gap_len.to_int())
                    );
            if( a_scaff.Valid() )
                scaffs.push_back(a_scaff);
            else
                break ;
        }
    }
    auto sout = BGIQD::FILES::FileWriterFactory
        ::GenerateWriterFromFileName(output.to_string());
    if(!sout)
        FATAL("failed to open output file to write !!! ");
    // Print scaff
    int index = 0 ;
    for( const auto & a_scaff : scaffs )
    {
        index ++ ;
        auto scaff_fa = a_scaff.ToReal(the_ref,index);
        *sout<<scaff_fa.head.Head()<<std::endl;
        *sout<<scaff_fa.seq.Seq(100);
    }
    delete sout;
    return 0;
}
