#!/bin/bash
# from wangwenchao@genomics.cn
# /nascngb/gccnt/wangou/assemblerdev/wangwenchao/sz_test/scripts/oo_pipeline.sh
minimap="/nascngb/gccnt/wangou/assemblerdev/wangwenchao/quast-master/quast_libs/minimap2/minimap2"
ont="/nascngb/gccnt/wangou/assemblerdev/tangjingbo/data/ONT/Chr21/rel3/Extract_Chr21_rel3.fa.gz"
contig="/nascngb/gccnt/wangou/assemblerdev/wangwenchao/sz_test/chr21/chr21.contig"
type_change="/hwfssz1/ST_OCEAN/USER/guolidong/ONTMapScaffoldAnalysis/wenchao_design/ONT_contigmapper"
oc="/hwfssz1/ST_OCEAN/USER/guolidong/ONTMapScaffoldAnalysis/wenchao_design/ONT_OrientationCorrecterV3"
scaff_infos="/nascngb/gccnt/wangou/assemblerdev/wangwenchao/sz_test/chr21/gap/chr21_new.scaff_infos"
filter_oo="/nascngb/gccnt/wangou/assemblerdev/wangwenchao/sz_test/scripts/filter_oo.py"
$minimap -c -x asm5 -B5 -O4,16 --no-long-join -r 85 -N 50 -s 65 -z 200 -t 40 $ont $contig 1>con2ont.paf 2>con2ont.log
awk '$12!=0 {print}' con2ont.paf >con2ont_nlq.paf
$type_change < con2ont_nlq.paf 1>cont2ont_4d.txt 2>contigmapper.log
$filter_oo --in cont2ont_4d.txt >cont2ont_4d_3o.txt
$oc $scaff_infos cont2ont_4d_3o.txt 1>new.scaffold_infos 2>orientation_correct.log

