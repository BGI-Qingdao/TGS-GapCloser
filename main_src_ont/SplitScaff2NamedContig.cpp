#include "biocommon/fasta/fasta.h"
#include "appcommon/ScaffInfo.h"
#include <string>
#include <sstream>

bool checkArgs( int argc , char **argv )
{
    if( argc > 1 )
    {
        std::string argv1(argv[1]);
        if( argv1  == "h" 
                ||  argv1  == "-h"
                ||  argv1  == "help"
                ||  argv1  == "--help" )
        {
            std::cerr<<"Usage : \n\t"
                <<argv[0]
                <<" <xxx.scaffseq >xxx.scafftig "
                <<std::endl;
            return false ;
        }
        else
        {
            std::cerr<<"ERROR : argument is not needed !! "<<std::endl;
            std::cerr<<"Usage : \n\t"
                <<argv[0]
                <<" <xxx.scaffseq >xxx.scafftig "
                <<std::endl;
            return false ;
        }
    }
    return true ;
}

int main(int argc , char **argv)
{
    if( !checkArgs( argc , argv ) )
    {
        return 0 ;
    }

    typedef BGIQD::FASTA::NormalHead Header ;
    typedef BGIQD::FASTA::Fasta<Header> Fa;

    std::vector<Fa> AllScaff;
    std::vector<Fa> AllScafftigs ;

    BGIQD::FASTA::FastaReader<Fa> Reader ;

    Reader.LoadAllFasta(std::cin ,AllScaff); 

    auto get_contig = [](long scaff_id ,long contig_id 
            , const std::string & seq ) -> Fa
    {
        Fa tmp ;
        std::ostringstream ost;
        ost<<">scaffold_"<<scaff_id<<"_contig_"<<contig_id;
        tmp.head.head =ost.str() ;
        tmp.seq.AddPartSeq(seq);
        return tmp ;
    };
    auto print_scaftig = [](const Fa & a_scaftig )
    {
        std::cout<<a_scaftig.head.Head()<<'\n';
        std::cout<<a_scaftig.seq.Seq(100);
    };

    long scaff_id = 0 ;
    for( const auto & a_scaff : AllScaff)
    {
        scaff_id ++ ;
        long contig_id = 0 ;
        const auto &seq = a_scaff.seq.atcgs ;
        std::string a_scaftig_agcts = "";
        int gap_size = 0 ;

        for( int i = 0 ; i < (int)seq.size() ; i ++ )
        {
            char x = seq[i];
            if( x == 'n' || x == 'N' )
                gap_size ++ ;
            else
            {
                if( gap_size > 0 )
                {
                    if( ! a_scaftig_agcts.empty() )
                    {
                        contig_id ++ ;
                        auto tmp = get_contig(scaff_id 
                                , contig_id
                                ,a_scaftig_agcts 
                                + std::string('N',gap_size));
                        print_scaftig(tmp);
                    }
                    a_scaftig_agcts.clear() ;
                    gap_size = 0 ;
                }
                a_scaftig_agcts.push_back(x);
            }

            if( ! a_scaftig_agcts.empty() )
            {
                assert(gap_size == 0 );
                contig_id ++ ;
                auto tmp = get_contig(scaff_id
                        , contig_id
                        , a_scaftig_agcts);
                print_scaftig(tmp);
            }
        }
    }
    return 0 ;
}
