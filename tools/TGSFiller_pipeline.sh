#!/bin/bash

###########################################################
# Step 1 :
#   edit configuration .
###########################################################

OUT_PREFIX="test_01"
BIN_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/source_dir/TGSGapFiller/bin"

CONTIG=""
SCAFF_INFO=""
READ12="/home/guolidong/chr19/chr19_reads12.clean.fastq"
ONT_FA="/home/xumengyang/ONT/chr19_eva/Extract_Chr19_rel3.fa"
MINIMAP2="/ldfssz1/ST_OCEAN/USER/xumengyang/software/minimap2/minimap2"
SAMTOOL="/home/softwares/samtools-1.9/bin/samtools"
JAVA="/home/softwares/jdk-12.0.1/jdk-12.0.1/bin/java"
PILON="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/xumengyang/software/pilon-1.23.jar"

PILON_MEM="300G"
###########################################################
# Step 2 :
#   generate sub-ont-fragments for all gaps .
###########################################################

$MINIMAP2  -x ava-ont -t 30 \
    $ONT_FA $CONTIG 1>$OUT_PREFIX.sub.paf 2>$OUT_PREFIX.minimap2.01.log

$BIN_DIR/ONTGapCandidate --ont_reads_a $ONT_FA --contig2ont_paf $OUT_PREFIX.sub.paf \
    <SCAFF_INFO >$OUT_PREFIX.gap_info_seqs 2>$OUT_PREFIX.cand.log

awk 'BEGIN{id=1;}{if($12 != "N"){gap_id=0+$1;printf(">seq_id_%d_%d\n%s\n",gap_id,id,$12);id+=1;}}'
    <$OUT_PREFIX.gap_info_seqs >$OUT_PREFIX.ont.fasta

###########################################################
# Step 2 :
#   pilon the sub-ont-fragments .
###########################################################

$MINIMAP2 -t 30 -d $OUT_PREFIX.mmi $ONT_FA 1>$OUT_PREFIX.minimap2.02.log 2>&1
$MINIMAP2 -t 30 -k14 -w5 -n2 -m20 -s 40 --sr --frag yes  --split-prefix=rel3_prefix \
    -a $OUT_PREFIX.mmi  $READ12  1>$OUT_PREFIX.sam 2>$OUT_PREFIX.minimap2.03.log

$BIN_DIR/samSQ_filter < $OUT_PREFIX.sam  >$OUT_PREFIX.fiter.sam

$SAMTOOL view -bo $OUT_PREFIX.bam  $OUT_PREFIX.fiter.sam -@ 30 \
    >$OUT_PREFIX.samtool_01.log 2>&1

$SAMTOOL sort $OUT_PREFIX.bam -o $OUT_PREFIX.sort.bam -@ 30 \
    >$OUT_PREFIX.samtool_02.log 2>&1

$SAMTOOL index $OUT_PREFIX.sort.bam -@ 30 \
    >$OUT_PREFIX.samtool_03.log 2>&1

$JAVA -Xmx$PILON_MEM -jar  $PILON --fix all \
    --genome $OUT_PREFIX.ont.pilon.fasta  --bam $OUT_PREFIX.sort.bam \
    --output $OUT_PREFIX --outdir ./ \
    --diploid  --threads 30 >$OUT_PREFIX.pilon.log 2>&1

###########################################################
# Step 3 :
#   use ont-sub-fragments to fill gaps
###########################################################


$MINIMAP2  -x ava-ont -t 30 $OUT_PREFIX.ont.pilon.fasta $CONTIG \
    1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log

$BIN_DIR/ONTGapFiller --ont_reads_a $OUT_PREFIX.ont.pilon.fasta \
    --contig2ont_paf $OUT_PREFIX.fill.paf --work_mode 4 <SCAFF_INFO \
    >$OUT_PREFIX.scaff_infos 2>$OUT_PREFIX.fill.log

if [[ ! -f $OUT_PREFIX.contig ]] ; then 
    ln -s $CONTIG $OUT_PREFIX.contig
else 
    echo "Warning : $OUT_PREFIX.contig already exist !!! use it anyway ."
fi

$BIN_DIR/ScaffInfo2Seq --prefix  $OUT_PREFIX --min_n 1 --min_c 0
