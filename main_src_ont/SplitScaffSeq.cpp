#include "biocommon/fasta/fasta.h"
#include "stLFR/ScaffInfo.h"
#include <string>

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
                <<" <xxx.scaffseq >xxx.scafftig 2>xxx.scaff_infos"
                <<std::endl;
            return false ;
        }
        else
        {
            std::cerr<<"ERROR : argument is not needed !! "<<std::endl;
            std::cerr<<"Usage : \n\t"
                <<argv[0]
                <<" <xxx.scaffseq >xxx.scafftig 2>xxx.scaff_infos"
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

    typedef BGIQD::FASTA::Id_Desc_Head  Header ;
    typedef BGIQD::FASTA::Fasta<Header> Fa;

    std::vector<Fa> AllScaff;
    std::vector<Fa> AllScafftigs ;

    BGIQD::FASTA::FastaReader<Fa> Reader ;

    Reader.LoadAllFasta(std::cin ,AllScaff); 

    BGIQD::stLFR::ScaffInfoHelper helper;

    auto get_contig = [](long contig_id 
            , const std::string & seq ) -> Fa
    {
        Fa tmp ;
        tmp.head.Id = std::to_string(contig_id);
        int contig_len = seq.size() ;
        auto curr_size = std::to_string(contig_len) ;
        tmp.head.Desc =" length "+curr_size+" cvg_1_tip_0";
        tmp.seq.AddPartSeq(seq);
        return tmp ;
    };


    auto get_contig_detail = [] (
            long contig_id,
            int contig_len ,
            int gap_size ,
            int scaff_id ,
            int scaff_index ,
            int start_pos 
            ) ->BGIQD::stLFR::ContigDetail 
    {

        BGIQD::stLFR::ContigDetail tmp ;
        tmp.contig_id = contig_id ;
        tmp.contig_len =  contig_len ;
        tmp.gap_size = gap_size ;
        tmp.orientation = true ;
        tmp.scaff_id = scaff_id ;
        tmp.scaff_index = scaff_index ;
        tmp.start_pos = start_pos ;
        return tmp ;
    };


    long index = 0 ;
    long scaff_id = 0 ;
    for( const auto & a_scaff : AllScaff)
    {
        scaff_id ++ ;
        const auto &seq = a_scaff.seq.atcgs ;
        std::string a_scaftig_agcts = "";
        bool n_prev = false ;
        int gap_size = 0 ;
        int contig_len = 0 ;
        int scaff_index = 0 ;
        int start_pos = 0 ;

        for( int i = 0 ; i < (int)seq.size() ; i ++ )
        {
            start_pos ++ ;
            char x = seq[i];
            bool gen_contig_detail = false ;
            bool gen_contig = false ;
            if( x == 'n' || x == 'N' )
            {
                if( ! n_prev )
                {
                    gap_size = 0 ;
                    if( ! a_scaftig_agcts.empty() )
                    {
                        index ++ ;
                        auto tmp = get_contig( index ,a_scaftig_agcts);
                        contig_len = a_scaftig_agcts.size();
                        AllScafftigs.push_back(tmp);
                        gen_contig = true ;
                    }
                }
                gap_size ++ ;
                n_prev = true ;
            }
            else
            {
                if( n_prev )
                {
                    scaff_index ++ ;
                    auto tmp = get_contig_detail( index , contig_len ,gap_size
                            ,scaff_id , scaff_index , start_pos ) ;
                    helper.all_scaff[scaff_id].scaff_id = scaff_id ;
                    helper.all_scaff[scaff_id].a_scaff.push_back(tmp);

                    contig_len = 0 ;
                    gap_size = 0 ;
                    gen_contig_detail = true ;
                    a_scaftig_agcts.clear() ;
                }
                a_scaftig_agcts.push_back(x);
                n_prev = false ;
            }

            if( i == (int)seq.size() -1 )
            { // this is the last scaftig of this scaffold
                if( !gen_contig )
                {
                    if( x != 'n' && x != 'N' )
                    {
                        index ++ ;
                        auto tmp = get_contig( index ,a_scaftig_agcts);
                        contig_len = a_scaftig_agcts.size();
                        AllScafftigs.push_back(tmp);
                        a_scaftig_agcts.clear() ;
                        gen_contig = true ;
                    }
                }
                if( !gen_contig_detail )
                {
                    scaff_index++ ;
                    auto tmp = get_contig_detail( index , contig_len ,gap_size
                            ,scaff_id , scaff_index , start_pos ) ;
                    helper.all_scaff[scaff_id].scaff_id = scaff_id ;
                    helper.all_scaff[scaff_id].a_scaff.push_back(tmp);

                    contig_len = 0 ;
                    gap_size = 0 ;
                    gen_contig_detail = true ;
                }
            }
        }
    }
    for( const  auto & a_scaftig : AllScafftigs )
    {
        std::cout<<a_scaftig.head.Head()<<'\n';
        std::cout<<a_scaftig.seq.Seq(100);
    }
    helper.PrintAllScaff(std::cerr);
}
