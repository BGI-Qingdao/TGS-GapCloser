#ifndef __COMMON_FLAGS_FLAGS_H__
#define __COMMON_FLAGS_FLAGS_H__

#include <bitset>

#define FLAGS_INT std::bitset<32> flags;

#define ADD_A_FLAG( index , name ) \
    void Set_##name() { flags.set(index) ;} \
    void Clean_##name() { flags.reset(index) ; }\
    void Flip_##name() { flags.flip(index) ; } \
    bool Is_##name() const { return flags.test(index) ; }

#endif //__COMMON_FLAGS_FLAGS_H__
