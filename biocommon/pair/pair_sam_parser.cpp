#include "biocommon/pair/pair_sam_parser.h"
#include "biocommon/sam_bam/sam_parser.h"
namespace BGIQD{
namespace SAM{

    std::pair<MatchData , MatchData> PairedSAMParser::CurrentPair()
    {
        MatchData P ;
        MatchData E ;
        std::string line;
        m_cache_next.clear();
        while( std::getline( m_ist , line ) )
        {
            LineParser l(line);
            if( l.IsHead() )
                continue;
            if( m_cache.empty())
            {
                m_cache.push_back(l.ParseAsMatchData());
                continue;
            }
            else
            {
                auto i = l.ParseAsMatchData() ;
                if( i.read_name == m_cache.begin()->read_name )
                {
                    m_cache.push_back(i);
                    continue;
                }
                else
                {
                    m_cache_next.push_back(i);
                    break;
                }
            }
        }
        // invalid now.
        if( m_cache.size() < 2 )
        {
            return std::make_pair(P,E);
        }
        // find correct PE
        for( const auto & i : m_cache )
        {
            if( i.IsP() && i.IsPrimaryMatch() )
                P = i ;
            if( i.IsE() && i.IsPrimaryMatch() )
                E = i ;
        }
        // swap cache
        m_cache.clear();
        m_cache_next.swap(m_cache);
        return std::make_pair(P,E);
    }

}// namespace SAM
}// namespace BGIQD
