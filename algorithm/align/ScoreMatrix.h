#ifndef __ALGORITHM_ALIGN_SCOREMATRIX_H__
#define __ALGORITHM_ALIGN_SCOREMATRIX_H__

#include <vector>

namespace BGIQD {
    namespace ALIGN {

        struct  Matrix
        {
            // 
            // Init memory .
            //  will generate a array[row+1][column+1]
            //
            //  0   1   2   3   4   5
            //
            //  1
            //
            //  2
            //
            //  3
            //
            //  4
            //
            //  5


            void Init( int row , int column ) 
            {
                len1 = row ;
                len2 = column ; 
                data.resize(Row() * Column());
            }
            int Row() const  { return len1 +1 ;}
            int Column() const  { return len2 +1 ;}

            //
            // the row & coloumn are all 0 base 
            //
            int Get( int row , int column ) const 
            {
                return  data.at(row * Column() + column) ;
            }

            void Set( int row , int column , int value)
            {
                data.at(row * Column() + column) =  value ;
            }

            private:
            int len1;
            int len2;
            std::vector<int> data;
        };
    }
}

#endif
