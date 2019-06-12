#ifndef __STLFR_ONT2GAP_H__
#define __STLFR_ONT2GAP_H__

#include "biocommon/paf/PAF.h"
#include "appcommon/contigPairInfo.h"

#include <vector>

namespace BGIQD {
    namespace ONT {

        struct ONT2GapInfo
        {
            BGIQD::PAF::PAF_Item from ;
            BGIQD::PAF::PAF_Item to ;
            BGIQD::stLFR::PairPN pair_info ;
        };

        typedef std::vector<ONT2GapInfo> ONT2GapInfos ;

        void SortMore( std::vector<ONT2GapInfo> & data );
        void SortLess( std::vector<ONT2GapInfo> & data );
        void SortMedian( std::vector<ONT2GapInfo> & data );
        void SortModifyLess( std::vector<ONT2GapInfo> & data );
        void SortNegModifyLess( std::vector<ONT2GapInfo> & data );
        void SortMatchScoreMore( std::vector<ONT2GapInfo> & data
                ,int max_hang 
                ,float fa = 1.0f
                ,float fb = 1.0f  );
    }
}

#endif //__STLFR_ONT2GAP_H__
