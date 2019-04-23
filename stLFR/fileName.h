#ifndef __SOAP2_FILENAME_H__
#define __SOAP2_FILENAME_H__

#include <string>
#include "common/string/stringtools.h"

namespace BGIQD {
    namespace SOAP2 {

        struct FileNames
        {
            void Init(const std::string & prefix)
            {
                m_prefix = prefix ;
            }

#define DEFINE_SUFFIX(name,suffix) \
            std::string name() const { return m_prefix + suffix ; } \
            std::string name(int round) const \
            {\
                if ( round == 0 )\
                {\
                    return name();\
                }\
                else\
                {\
                    return m_prefix + suffix +"_round_"+BGIQD::STRING::itos(round) ;\
                }\
            }\
            std::string name(std::string middle) const \
            {\
                if( middle == "" )\
                {\
                    return name() ;\
                }\
                else\
                {\
                    return m_prefix + "." + middle+  suffix ;\
                }\
            }\

            // SOAP2
            DEFINE_SUFFIX(Arc,".Arc");

            DEFINE_SUFFIX(updatedEdge,".updated.edge");

            DEFINE_SUFFIX(contig, ".contig");
            DEFINE_SUFFIX(contig_short, ".contig_short");
            DEFINE_SUFFIX(contig_long, ".contig_long");

            DEFINE_SUFFIX(ContigIndex, ".ContigIndex");
            // StaticUnique
            DEFINE_SUFFIX(seeds, ".seeds");
            DEFINE_SUFFIX(mst_seeds, ".mst_seeds");
            DEFINE_SUFFIX(mst_pe_seeds, ".mst_pe_seeds");
            DEFINE_SUFFIX(cdg_seeds, ".cdg_seeds");
            DEFINE_SUFFIX(pe_seeds, ".pe_seeds");
            // Same2ReadOnContig
            DEFINE_SUFFIX(read2contig_sam, ".read2contig.sam");

            DEFINE_SUFFIX(contig2r1_sam,".contig2r1.sam");
            DEFINE_SUFFIX(contig2r2_sam,".contig2r2.sam");

            DEFINE_SUFFIX(read2contig, ".read2contig");
            DEFINE_SUFFIX(read2contig_v1, ".read2contig_v1");
            DEFINE_SUFFIX(contig2read_v1,".contig2read_v1");
            DEFINE_SUFFIX(contig2r1_v1,".contig2r1_v1");
            DEFINE_SUFFIX(contig2r2_v1,".contig2r2_v1");
            DEFINE_SUFFIX(contig_pe_conns,".contig_pe_conns");
            // PEGraph 
            DEFINE_SUFFIX(pe_graph , ".pe_graph");

            DEFINE_SUFFIX(pe_info,".pe_info");
            DEFINE_SUFFIX(pe_pairs,".pe_pair");
            DEFINE_SUFFIX(pe_singles,".pe_singles");
            DEFINE_SUFFIX(pe_boths,".pe_boths");

            // ChopBin
            DEFINE_SUFFIX(barcodeList, ".barcodeList");
            DEFINE_SUFFIX(barcodeFreq, ".barcodeFreq");
            DEFINE_SUFFIX(readNameList, ".readNameList");

            DEFINE_SUFFIX(BarcodeOnBin, ".barcodeOnBin");

            DEFINE_SUFFIX(BarcodeOnContig, ".barcodeOnContig");

            DEFINE_SUFFIX(BarcodeOnContig_fake, ".barcodeOnContig_fake");

            DEFINE_SUFFIX(contigOnBarcode,".contigOnBarcode");

            DEFINE_SUFFIX(contig_at_barcode_v1,".contig_at_barcode_v1");

            DEFINE_SUFFIX(barcode_at_contig_v1 , ".barcode_at_contig_v1");

            // Bin Cluster
            DEFINE_SUFFIX(cluster, ".cluster");

            // MST
            DEFINE_SUFFIX(mintree, ".mintree");

            DEFINE_SUFFIX(mintreetrunk, ".mintree_trunk");

            DEFINE_SUFFIX(mintreetrunklinear, ".mintree_trunk_linear");
            DEFINE_SUFFIX(mst_error, ".mst_error");

            DEFINE_SUFFIX(bin_cluster, ".bin_cluster");
            // ContigDLink
            DEFINE_SUFFIX(connInfo, ".connInfo");
            DEFINE_SUFFIX(connPath, ".connPath");
            // LinearCDG
            DEFINE_SUFFIX(contigroad , ".contigroad");
            // FillContigRoad
            DEFINE_SUFFIX(contigroadfill,".contigroadfill");
            // MergeContig
            DEFINE_SUFFIX(super_used, ".super_used");

            DEFINE_SUFFIX(super_only, ".super_only");

            DEFINE_SUFFIX(super_and_left, ".super_and_left");

            //ClusterSeeds
            DEFINE_SUFFIX(seeds_cluster_seeds,".seeds_cluster_seeds");

            DEFINE_SUFFIX(gap_oo,".gap_oo");
            DEFINE_SUFFIX(gap_area,".gap_area");
            DEFINE_SUFFIX(gap_sim,".gap_sim");

            DEFINE_SUFFIX(scaff_seqs,".scaff_seqs");
            DEFINE_SUFFIX(scaff_infos,".scaff_infos");
            DEFINE_SUFFIX(scaff_gap2filler_seqs,".scaff_gap2filler_seqs");

            DEFINE_SUFFIX(trunk_fill,".trunk_fill");

            DEFINE_SUFFIX(seed_extern_fill ,".seed_extern_fill");

            DEFINE_SUFFIX(barcodeOnScaff, ".barcodeOnScaff");

            DEFINE_SUFFIX(barcodeOnGaps_intersection, ".barcodeOnGaps_intersection");
            DEFINE_SUFFIX(barcodeOnGaps_union, ".barcodeOnGaps_union");

            private:
                std::string m_prefix;
        };
    }//namespace SOAP2
}//namespace BGIQD

#endif //__SOAP2_FILENAME_H__
