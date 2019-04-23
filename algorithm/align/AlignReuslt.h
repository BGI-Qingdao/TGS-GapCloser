#ifndef __ALGORITHM_ALIGN_ALIGNRESULT_H__
#define __ALGORITHM_ALIGN_ALIGNRESULT_H__

namespace BGIQD {
    namespace ALIGN {

        enum ResultType
        {
            Unknow = 0 ,
            LeftTop_Match = 1 ,
            LeftTop_UnMatch = 2 ,
            Left = 3 , // delete 
            Top = 4 , // insert 
        };

        struct AlignResult
        {
            int row_id ;
            int column_id ;
            ResultType type ;
        };
    }
}
#endif
