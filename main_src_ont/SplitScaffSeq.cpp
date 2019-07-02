#include "biocommon/fasta/fasta.h"
#include "appcommon/ScaffInfo.h"
#include "appcommon/fileName.h"
#include "common/args/argsparser.h"
#include "common/files/file_reader.h"
#include "common/files/file_writer.h"
#include "common/error/Error.h"
#include <string>

int main(int argc , char **argv)
{
    START_PARSE_ARGS
    DEFINE_ARG_REQUIRED(std::string , input_scaff , " the input scaffold fasta file");
    DEFINE_ARG_REQUIRED(std::string , prefix , " the prefix of output files , will output :\n\
                                prefix.contig\n\
                                prefix.orignial_scaff_infos\n\
                                prefix.name_map");
    END_PARSE_ARGS

    BGIQD::SOAP2::FileNames fNames;
    fNames.Init(prefix.to_string() );

    typedef BGIQD::FASTA::Id_Desc_Head  Header ;
    typedef BGIQD::FASTA::Fasta<Header> Fa;

    std::vector<Fa> AllScaff;
    std::vector<Fa> AllScafftigs ;

    BGIQD::FASTA::FastaReader<Fa> Reader ;

    // Load the input scaff 
    auto in_scaff = BGIQD::FILES::FileReaderFactory::GenerateReaderFromFileName(input_scaff.to_string() );
    if( in_scaff  == NULL )
        FATAL(" failed to open input_scaff for read!!!");
    Reader.LoadAllFasta(*in_scaff ,AllScaff);
    delete in_scaff ;

    // parse the input scaff 
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
    std::map<long , std::string > name_map ;
    for( const auto & a_scaff : AllScaff)
    {
        scaff_id ++ ;
        name_map[scaff_id] = a_scaff.head.Id;
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

    // print contig
    auto contig_f= BGIQD::FILES::FileWriterFactory::GenerateWriterFromFileName(fNames.contig());
    if( contig_f== NULL )
        FATAL( "failed to open xxx.contig for write !!!");
    for( const  auto & a_scaftig : AllScafftigs )
    {
        (*contig_f)<<a_scaftig.head.Head()<<'\n';
        (*contig_f)<<a_scaftig.seq.Seq(100);
    }
    delete contig_f;
    // print orignial_scaff_infos
    helper.FormatAllIndex();
    helper.FormatAllStartPos();
    auto out_scaff_infos = BGIQD::FILES::FileWriterFactory::GenerateWriterFromFileName(fNames.orignial_scaff_infos());
    if( out_scaff_infos == NULL )
        FATAL( "failed to open xxx.orignial_scaff_infos for write !!!");
    helper.PrintAllScaff(*out_scaff_infos);
    delete out_scaff_infos ;

    // print name_map
    auto name_map_f = BGIQD::FILES::FileWriterFactory::GenerateWriterFromFileName(fNames.name_map());
    if(name_map_f == NULL )
        FATAL( "failed to open xxx.name_map for write !!!");
    for( const auto & p : name_map )
        (*name_map_f)<<p.first<<'\t'<<p.second<<'\n';
    delete name_map_f ;
}
