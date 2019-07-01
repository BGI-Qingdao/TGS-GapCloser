#!/bin/bash

###########################################################
# Step 0 :
#   edit configuration .
###########################################################

#######################################
# basic input files.
#######################################
# the input scaffold sequence file
INPUT_SCAFF_FA="xxx"
# the Third generation long reads ( raw data here )
ONT_FA="/home/xumengyang/ONT/chr19_eva/Extract_Chr19_rel3.fa"
# NGS reads to polish above long reads. please concatenate read1 and read2 into 1 file. 
READ12="/home/guolidong/chr19/chr19_reads12.clean.fastq"

#######################################
# basic project settings.
#######################################

# prefix of output files.
OUT_PREFIX="gapfill_test"
# java memory alloced for pilon running time.
PILON_MEM="300G"
# pilon chunk num.
CHUNK_NUM=1

#######################################
# basic 3rd part tools settings.
#######################################

# this is a directory
BIN_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/source_dir/TGSGapFiller/bin"
# this is a executive file 
MINIMAP2="/home/softwares/minimap2-master/minimap2"
# this is a executive file 
SAMTOOL="/home/softwares/samtools-1.9/bin/samtools"
# this is a executive file 
JAVA="/home/softwares/jdk-12.0.1/jdk-12.0.1/bin/java"
# this is a jar file 
PILON="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/xumengyang/software/pilon-1.23.jar"

###########################################################
# Step 1 :
#   preprocess input file
###########################################################

TMP_INPUT_SCAFTIG=$OUT_PREFIX".contig"
TMP_INPUT_SCAFF_INFO=$OUT_PREFIX".input.scaff_infos"

$BIN_DIR/SplitScaffSeq <$INPUT_SCAFF_FA >$TMP_INPUT_SCAFTIG 2>$TMP_INPUT_SCAFF_INFO
###########################################################
# Step 2 :
#   generate sub-ont-fragments for all gaps .
###########################################################

$MINIMAP2  -x ava-ont -t 30 --sam-hit-only \
    $ONT_FA $TMP_INPUT_SCAFTIG 1>$OUT_PREFIX.sub.paf 2>$OUT_PREFIX.minimap2.01.log || exit 1

$BIN_DIR/ONTGapCandidate --ont_reads_a $ONT_FA --contig2ont_paf $OUT_PREFIX.sub.paf \
    <$SCAFF_INFO >$OUT_PREFIX.gap_info_seqs 2>$OUT_PREFIX.cand.log || exit 1

for ((i=0; i<COV; i++))
do
    awk 'BEGIN{id=1;ii=0;prev=0}{if($12 != "N"){ gap_id=0+$1;ii=ii+1 ; if(prev!=gap_id){prev=gap_id;ii=1;}if(ii%(cov)==the_i){printf(">seq_id_%d_%d\n%s\n",gap_id,id,$12);}id+=1;}}' cov=$COV the_i=$i \
        <$OUT_PREFIX.gap_info_seqs >$OUT_PREFIX.ont.$i.fasta || exit 1
done

###########################################################
# Step 3 :
#   pilon the sub-ont-fragments .
###########################################################
for ((i=0; i<COV; i++))
do
    $MINIMAP2 -t 30 -d $OUT_PREFIX.mmi $OUT_PREFIX.ont.$i.fasta 1>$OUT_PREFIX.minimap2.02.log 2>&1 || exit 1
    $MINIMAP2 -t 30 -k14 -w5 -n2 -m20 -s 40 --sr --frag yes  --split-prefix=rel3_prefix --sam-hit-only \
        -a $OUT_PREFIX.mmi  $READ12  1>$OUT_PREFIX.sam 2>$OUT_PREFIX.minimap2.03.log || exit 1
    awk -f $BIN_DIR/samSQ_filter.awk < $OUT_PREFIX.sam  >$OUT_PREFIX.fiter.sam || exit 1
    #$BIN_DIR/samSQ_filter < $OUT_PREFIX.sam  >$OUT_PREFIX.fiter.sam || exit 1

    $SAMTOOL view -bo $OUT_PREFIX.bam  $OUT_PREFIX.fiter.sam -@ 30 \
        >$OUT_PREFIX.samtool_01.log 2>&1 || exit 1

    $SAMTOOL sort $OUT_PREFIX.bam -o $OUT_PREFIX.sort.bam -@ 30 \
        >$OUT_PREFIX.samtool_02.log 2>&1 || exit 1

    $SAMTOOL index $OUT_PREFIX.sort.bam -@ 30 \
        >$OUT_PREFIX.samtool_03.log 2>&1 || exit 1

    $JAVA -Xmx$PILON_MEM -jar  $PILON --fix all \
        --genome $OUT_PREFIX.ont.$i.fasta --bam $OUT_PREFIX.sort.bam \
        --output $OUT_PREFIX.ont.$i.pilon --outdir ./ \
        --diploid  --threads 30 >$OUT_PREFIX.pilon.log 2>$OUT_PREFIX.pilon.err || exit 1
done
for ((i=0; i<COV; i++))
do
    cat $OUT_PREFIX.ont.$i.pilon.fasta >> $OUT_PREFIX.ont.pilon.fasta
done

###########################################################
# Step 4 :
#   use ont-sub-fragments to fill gaps
###########################################################


$MINIMAP2  -x ava-ont -t 30 $OUT_PREFIX.ont.pilon.fasta $TMP_INPUT_SCAFTIG --sam-hit-only \
    1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1

$BIN_DIR/ONTGapFiller --ont_reads_a $OUT_PREFIX.ont.pilon.fasta \
    --contig2ont_paf $OUT_PREFIX.fill.paf --contig $TMP_INPUT_SCAFTIG <$TMP_INPUT_SCAFF_INFO\
    >$OUT_PREFIX.scaff_infos 2>$OUT_PREFIX.fill.log || exit 1

$BIN_DIR/ScaffInfo2Seq --prefix  $OUT_PREFIX --min_n 1 --min_c 0 \
    2>$OUT_PREFIX.i2s.log 2>&1  || exit 1
