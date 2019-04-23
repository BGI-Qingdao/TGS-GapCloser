#include "biocommon/seq/seq.h"
#include "biocommon/seq/tool_func.h"
#include <sstream>
namespace BGIQD {
    namespace SEQ {

            std::string seq::ReverseCompleteSeq(int weight) const
            {
                std::string r_atcgs = seqCompleteReverse(atcgs);
                if( weight < 1 )
                    return r_atcgs ;
                else
                {
                    std::ostringstream ost;
                    int i = 1 ;
                    for( char c : r_atcgs)
                    {
                        ost<<c;
                        if( i % weight == 0 || i == (int)r_atcgs.size() )
                            ost<<'\n';
                        i++ ;
                    }
                    return ost.str();
                }
            }

            std::string seq::Seq(int weight ) const
            {
                if( weight < 1 )
                    return atcgs ;
                else
                {
                    std::ostringstream ost;
                    int i = 1 ;
                    for( char c : atcgs)
                    {
                        ost<<c;
                        if( i % weight == 0 || i == (int)atcgs.size() )
                            ost<<'\n';
                        i++ ;
                    }
                    return ost.str();
                }
            }
    }
}
