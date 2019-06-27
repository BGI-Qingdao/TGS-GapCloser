#ifndef __APPCOMMON_ALIGNMINIMAP2_H__
#define __APPCOMMON_ALIGNMINIMAP2_H__

#include "minimap2/minimap.h"
#include <string>
#include <cstring>
#include <cassert>
#include <iostream>
namespace BGIQD {
    namespace ALIGN_MINIMAP2 {

        const int max_overlap_size =10000;

        int CheckOverlap(const std::string & left , /*ref*/
                const std::string & right ,        /*query*/
                float aligned_factor,
                float idy_factor,
                int overlap_len
                )
        {
            int ref_len = left.size() ;
            // prepare options
            mm_idxopt_t iopt;
            mm_mapopt_t mopt;
            mm_verbose = 2; // disable message output to stderr
            mm_set_opt(0, &iopt, &mopt);
            mopt.flag |= MM_F_CIGAR; // perform alignment
            // index ref
            const char *fake_name = "N/A";
            char *s;
            s = (char*)calloc(left.size() + 1, 1);
            memcpy(s, left.c_str(), left.size());
            mm_idx_t *mi = mm_idx_str(1, /*-w*/  /* same as minimap2 default*/
                    11,                   /*-k*/
                    0,                    /*-H*/
                    14,                   /*bucket-bits*/
                    1,                    /*number of seq*/
                    (const char**)&s,     /*seq*/
                    (const char**)&fake_name /* name ,could be NULL */
                    );
            free(s); s=NULL ;
            mm_mapopt_update(&mopt, mi); 
            mm_tbuf_t *tbuf = mm_tbuf_init(); 

            mm_reg1_t *reg;
            int j,  n_reg;
            mopt.min_chain_score = 1; // -m 1
            reg = mm_map(mi, right.size(), right.c_str(), &n_reg, tbuf, &mopt, 0);
            int ret = -1 ;
            for (j = 0; j < n_reg; ++j) 
            {

                mm_reg1_t *r = &reg[j];
                assert(r->p); // with MM_F_CIGAR, this should not be NULL
                std::cerr<<overlap_len<<'\t'
                    <<ref_len<<'\t'
                    <<r->blen<<'\t'
                    <<float(r->blen) / float(ref_len) <<'\t'
                    <<float(r->mlen) / float(r->blen) <<'\n';
                if( float(r->mlen) / float(r->blen) >=idy_factor 
                &&  float(r->blen) / float(overlap_len) >=aligned_factor
                && ret < 0)
                {
                    ret = r->rs ;
                    std::cerr<<" accept this overlap "<<'\n';
                }

                free(r->p);
            }
            free(reg);
            mm_tbuf_destroy(tbuf);
            mm_idx_destroy(mi);
            return ret ;
        }
    }
}
#endif
