#ifndef __ALGORITHM_LINEAR_FITTING_LINEAR_H__
#define __ALGORITHM_LINEAR_FITTING_LINEAR_H__

namespace BGIQD {
    namespace LINEARFITTING {

        template<class X , class Y>
            struct Linear
            {
                // 0 = ax + by + c
                double a ;
                double b ;
                double c ;

                Y getY(const X & x ) const 
                {
                    return ( -a *x -c ) / b ;
                }
                X getX(const Y & y ) const 
                {
                    return ( -b * y - c ) / a ;
                }
            };



    }
}

#endif //__ALGORITHM_LINEAR_FITTING_LINEAR_H__
