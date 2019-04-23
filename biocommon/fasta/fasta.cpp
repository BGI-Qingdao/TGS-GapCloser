#include "biocommon/fasta/fasta.h"

#include <string>
#include <sstream>
namespace BGIQD {

    namespace FASTA {

        void SOAP2ContigHead::Init( const std::string & line ) 
        {
            //>21 length 64 cvg_0.0_tip_0
            sscanf(line.c_str() 
                    ,">%u length %d cvg_%f_tip_%d"
                    ,&contigId
                    ,&len
                    ,&cov 
                    ,&is_tip);

        }

        std::string SOAP2ContigHead::Head() const {
            std::ostringstream ost;
            ost<<'>'<<contigId
                <<" length "<<len
                <<" cvg_"<<cov
                <<"_tip_"<<is_tip;
            return ost.str();
        }


        void ScaffSplitGapHead::Init(const std::string & line)
        {
            int t ;
            sscanf(line.c_str() 
                    ,">%d_%d\t%u\t%u\t%u\t%u\t%d"
                    ,&scaff_id
                    ,&gap_index
                    ,&prev_base_contig
                    ,&next_base_contig
                    ,&prev_contig
                    ,&next_contig
                    ,&t
                  );
            gap_type = static_cast<GapType>(t);
        }

        std::string ScaffSplitGapHead::Head() const 
        {
            std::ostringstream ost;
            ost<<'>'<<scaff_id<<'_'<<gap_index
                <<'\t'<<prev_base_contig
                <<'\t'<<next_base_contig
                <<'\t'<<prev_contig
                <<'\t'<<next_contig
                <<'\t'<<int(gap_type);
            return ost.str();
        }

        void Id_Desc_Head::Init( const std::string &line )
        {
            Reset() ;
            bool turn = false ;
            assert(line.size() > 1);
            assert(line[0] == '>');
            for( int i = 1 ; i < (int)line.size() ; i++ )
            {
                if( !turn  )
                {
                    if( ! std::isblank(line[i]) )
                        Id += line[i] ;
                    else 
                        turn = true ;
                }
                if( turn )
                    Desc += line[i] ;
            }
        }
    }
}
