#ifndef __ALGORITHM_ALIGN_NEEDLEMANWUNSCH_H__
#define __ALGORITHM_ALIGN_NEEDLEMANWUNSCH_H__

#include "algorithm/align/ScoreSchemes.h"
#include "algorithm/align/ScoreMatrix.h"
#include "algorithm/align/AlignReuslt.h"

#include <vector>
#include <stack>

namespace BGIQD {
    namespace ALIGN {

        template<class T>
            struct NeedlemanWunsch
            {
                typedef T Item;
                std::vector<Item> data1;
                std::vector<Item> data2;
                BGIQD::ALIGN::Matrix the_matrix ;
                BGIQD::ALIGN::Schemes schemes;

                void InitAfterDataLoaded() ;
                void FillMutrix();
                std::stack<BGIQD::ALIGN::AlignResult> GetResult() const ; 
            };
    }
}

#endif
