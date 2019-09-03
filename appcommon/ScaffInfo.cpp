#include "appcommon/ScaffInfo.h"
#include "common/string/stringtools.h"

#include <cassert>
#include <sstream>
namespace BGIQD {
    namespace stLFR {

        const std::string ContigDetail::ONT_FILL = "ONT_FILL" ;
        const std::string ContigDetail::GAP_TYPE = "GAP_TYPE" ;

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
            while( ! ist.eof() )
            {
                ist>>extra_item ;
                auto items = BGIQD::STRING::split(extra_item,"=");
                assert(items.size() == 2 ) ;
                extra[items[0]] = items[1] ;
            }
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
            for( const auto & pair : extra )
            {
                ost<<'\t'<<pair.first<<'='<<pair.second ;
            }
            return ost.str();
        }

        void ScaffInfo::FormatStartPos()
        {
            int start_pos= 1 ;
            for( auto & contigd : a_scaff )
            {
                contigd.start_pos = start_pos;
                start_pos += contigd.contig_len ;
                start_pos += contigd.gap_size ;
                if( contigd.scaff_id != scaff_id )
                    contigd.scaff_id = scaff_id ;
            }
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

        void ScaffInfo::PrintScaff(  std::ostream &ost ) const
        {
            ost<<">scaffold "<<scaff_id<<'\n';
            for(const auto & i : a_scaff)
            {
                ost<<i.ToString()<<'\n';
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
