#!/bin/bash

###########################################################
# Step 1 :
#   edit configuration .
###########################################################

OUT_PREFIX="test_01"
BIN_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/source_dir/TGSGapFiller/bin"

CONTIG="/home/guolidong/stLFR_debug/agp_test/test03/test_ontfill.contig"
SCAFF_INFO="/home/guolidong/stLFR_debug/agp_test/test03/test_ontfill.scaff_infos_0"
READ12="/home/guolidong/chr19/chr19_reads12.clean.fastq"
ONT_FA="/home/xumengyang/ONT/chr19_eva/Extract_Chr19_rel3.fa"
MINIMAP2="/home/softwares/minimap2/minimap2"
RACON="/home/softwares/racon/build/bin/racon"
ROUND=3
###########################################################
# Step 2 :
#   generate sub-ont-fragments for all gaps .
###########################################################

$MINIMAP2  -x ava-ont -t 30 \
    $ONT_FA $CONTIG 1>$OUT_PREFIX.sub.paf 2>$OUT_PREFIX.minimap2.01.log || exit 1
echo "---- 1 ------ "
$BIN_DIR/ONTGapCandidate --ont_reads_a $ONT_FA --contig2ont_paf $OUT_PREFIX.sub.paf \
    <$SCAFF_INFO >$OUT_PREFIX.gap_info_seqs 2>$OUT_PREFIX.cand.log || exit 1
echo "---- 2 ------ "
awk 'BEGIN{id=1;}{if($12 != "N"){gap_id=0+$1;printf(">seq_id_%d_%d\n%s\n",gap_id,id,$12);id+=1;}}' \
    <$OUT_PREFIX.gap_info_seqs >$OUT_PREFIX.ont.fasta || exit 1
echo "---- 3 ------ "
###########################################################
# Step 2 :
#   racon the sub-ont-fragments .
###########################################################

$MINIMAP2 -t 40 -d $OUT_PREFIX.mmi $ONT_FA

$MINIMAP2 -t 40 -k14 -w5 -n2 -m20 -s 40 --sr --frag yes --split-prefix=rel3_prefix \
    $OUT_PREFIX.mmi $R1 $R2 1>$OUT_PREFIX.racon.paf 2>$OUT_PREFIX.minimap2.01.log

$RACON -t 40 $READ12 $OUT_PREFIX.paf $ONT_FA  1> $OUT_PREFIX.racon.1.fa  \
    2> $OUT_PREFIX.racon.1.log

for ((i=1; i<ROUND; i++))
do
    echo "round + $i"
    let next=i+1;
    $MINIMAP2 -t 40 -d $OUT_PREFIX.mmi $OUT_PREFIX.racon.$i.fa
    $MINIMAP2 -t 40 -k14 -w5 -n2 -m20 -s 40 --sr --frag yes --split-prefix=rel3_prefix \
        $OUT_PREFIX.mmi $R1 $R2 1>$OUT_PREFIX.racon.paf 2>$OUT_PREFIX.minimap2.02.log
    $RACON -t 40 $READ12 $OUT_PREFIX.paf $OUT_PREFIX.racon.$i.fa  \
        1> $OUT_PREFIX.racon.$next.fa  2> $OUT_PREFIX.racon.1.log
done
###########################################################
# Step 3 :
#   use ont-sub-fragments to fill gaps
###########################################################
$MINIMAP2  -x ava-ont -t 30 $OUT_PREFIX.racon.$ROUND.fa  $CONTIG \
    1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1
echo "---- 31 ------ "

$BIN_DIR/ONTGapFiller --ont_reads_a $OUT_PREFIX.racon.$ROUND.fa \
    --contig2ont_paf $OUT_PREFIX.fill.paf --work_mode 4 <$SCAFF_INFO \
    >$OUT_PREFIX.scaff_infos 2>$OUT_PREFIX.fill.log || exit 1
echo "---- 32 ------ "
if [[ ! -f $OUT_PREFIX.contig ]] ; then
    ln -s $CONTIG $OUT_PREFIX.contig || exit 1
else
    echo "Warning : $OUT_PREFIX.contig already exist !!! use it anyway ."
fi
echo "---- 33 ------ "
$BIN_DIR/ScaffInfo2Seq --prefix  $OUT_PREFIX --min_n 1 --min_c 0 \
 >$OUT_PREFIX.i2s.log 2>&1  || exit 1
echo "---- Done ------ "
