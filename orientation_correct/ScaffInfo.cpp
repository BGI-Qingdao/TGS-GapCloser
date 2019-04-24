#include "ScaffInfo.h"

#include <cassert>
#include <sstream>
namespace BGIQD {
    namespace stLFR {

        void ContigDetail::InitFromString(const std::string &line)
        {
            std::istringstream ist(line);
            char pos ;
            ist >>contig_id
                >>pos
                >>gap_size
                >>contig_len
                >>start_pos
                >>scaff_index
                >>scaff_id;
            orientation = ( pos == '+' );
            std::string extra_item ;
        }

        std::string ContigDetail::ToString() const 
        {
            std::ostringstream ost;

            ost <<contig_id<<'\t'
                <<(orientation ? '+' : '-')<<'\t'
                <<gap_size<<'\t'
                <<contig_len<<'\t'
                <<start_pos<<'\t'
                <<scaff_index<<'\t'
                <<scaff_id;
            return ost.str();
        }

        void ScaffInfo::PrintScaff(  std::ostream &ost ) const
        {
            ost<<">scaffold"<<scaff_id<<'\n';
            for(const auto & i : a_scaff)
            {
                ost<<i.ToString()<<'\n';
            }
        }

        void ScaffInfo::FormatStartPos()
        {

        }
        void ScaffInfo::FormatIndex()
        {
            int index = 1 ;
            for( auto & contigd : a_scaff )
            {
                contigd.scaff_index = index ;
                index ++ ;
                if( contigd.scaff_id != scaff_id )
                    contigd.scaff_id = scaff_id ;
            }
        }

        void ScaffInfoHelper::PrintAllScaff(std::ostream &ost ) const
        {
            for( const auto & pair :all_scaff )
            {
                pair.second.PrintScaff(ost);
            }
        }

        void ScaffInfoHelper::LoadAllScaff(  std::istream &ist )
        {
            int id ;
            std::string line ;
            while( ! std::getline( ist , line).eof() )
            {
                if( line[0] == '>' )
                {
                    sscanf(line.c_str(),">scaffold %d",&id);
                    all_scaff[id].scaff_id = id ;
                }
                else
                {
                    ContigDetail tmp ;
                    tmp.InitFromString(line);
                    all_scaff[id].a_scaff.push_back(tmp);
                    contig_indexs[tmp.contig_id] = ContigIndex{ id ,int( all_scaff[id].a_scaff.size() -1) } ;
                }
            }
        }

        ContigDetail & ScaffInfoHelper::GetContig( unsigned int contig)
        {
            try
            {
                auto index = contig_indexs.at(contig);
                return all_scaff.at(index.scaffold_id).a_scaff.at(index.contig_index);
            }
            catch(...)
            {
                assert(0);
            }
            assert(0);
            static ContigDetail invalid ;
            return invalid ;;
        }
    }
}
