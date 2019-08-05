#!/bin/bash

###########################################################
# Step 0 :
#   edit configuration . please double check every detail.
###########################################################

#######################################
# basic input files.
#######################################
# the input scaffold sequence file
INPUT_SCAFF_FA="xxx"
# the Third generation long reads ( raw data here )
TGS_FA="/home/xumengyang/ONT/chr19_eva/Extract_Chr19_rel3.fa"
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
# cpu for minimap2 & samtools & pilon
CPU=30
#minimp2 parameters
MINIMAP2_PARAM=" -x ava-ont "
#filter for TGSGapFiiler
MinIDY="0.3"
MinMatch=300

#######################################
# basic tools settings.
#######################################

# this is a directory
TGSGapFiller_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/TGSGapFiller"
# this is a executive file 
SAMTOOL="/home/softwares/samtools-1.9/bin/samtools"
# this is a executive file 
JAVA="/home/softwares/jdk-12.0.1/jdk-12.0.1/bin/java"
# this is a jar file 
PILON="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/xumengyang/software/pilon-1.23.jar"

###########################################################
# Step 0.5 :
#   sanity check . normally do not modify any code below.
###########################################################
## check subdir here. below
BIN_DIR=$TGSGapFiller_DIR"/bin"
MINIMAP2=$TGSGapFiller_DIR"/minimap2/minimap2"

if [[ ! -d $BIN_DIR || ! -e $MINIMAP2 ]] ; then 
    echo "ERROR :$BIN_DIR is not exist or $MINIMAP2 is not executable !!! exit ..."
    exit 1;
fi

if [[ ! -e $SAMTOOL ]] ; then 
    echo "ERROR : $SAMTOOL is not executable !!! exit ..."
    exit 1;
fi

if [[ ! -e $JAVA ]] ; then 
    echo "ERROR : $JAVA is not executable !!! exit ..."
    exit 1;
fi
if [[ ! -f $PILON ]] ; then 
    echo "ERROR : $PILON is not exist  !!! exit ..."
    exit 1;
fi

###########################################################
# Step 1 :
#   preprocess input file
###########################################################

TMP_INPUT_SCAFTIG=$OUT_PREFIX".contig"
TMP_INPUT_SCAFF_INFO=$OUT_PREFIX".orignial_scaff_infos"

$BIN_DIR/TGSSeqSplit --input_scaff $INPUT_SCAFF_FA \
    --prefix $OUT_PREFIX 2>$OUT_PREFIX.seq_split.log  || exit 1
###########################################################
# Step 2 :
#   generate sub-ont-fragments for all gaps .
###########################################################

$MINIMAP2  $MINIMAP2_PARAM  -t $CPU  \
    $TGS_FA $TMP_INPUT_SCAFTIG \
    1>$OUT_PREFIX.sub.paf 2>$OUT_PREFIX.minimap2.01.log || exit 1

$BIN_DIR/TGSGapCandidate --ont_reads_a $TGS_FA \
    --contig2ont_paf $OUT_PREFIX.sub.paf \
    --candidate_max 10 --candidate_shake_filter \
    --candidate_merge \
    <$TMP_INPUT_SCAFF_INFO >$OUT_PREFIX.ont.fasta \
    2>$OUT_PREFIX.cand.log || exit 1

if [[ $CHUNK_NUM != "1" ]] ; then
    for ((i=0; i<CHUNK_NUM; i++))
    do
        awk 'BEGIN{id=0}{if($1 ~ />/ ){id=id+1;} if(id % cov == the_i) {print $0;} }' cov=$CHUNK_NUM the_i=$i \
            <$OUT_PREFIX.ont.fasta >$OUT_PREFIX.ont.$i.fasta || exit 1
    done
else
    ln -s $OUT_PREFIX.ont.fasta $OUT_PREFIX.ont.0.fasta
fi
###########################################################
# Step 3 :
#   pilon the sub-ont-fragments .
###########################################################
for ((i=0; i<CHUNK_NUM; i++))
do
    $MINIMAP2 -t $CPU -d $OUT_PREFIX.mmi $OUT_PREFIX.ont.$i.fasta \
        1>$OUT_PREFIX.minimap2.02.log 2>&1 || exit 1
    $MINIMAP2 -t $CPU -k14 -w5 -n2 -m20 -s 40 --sr --frag yes  \
        --split-prefix=rel3_prefix \
        -a $OUT_PREFIX.mmi  $READ12  \
        1>$OUT_PREFIX.sam 2>$OUT_PREFIX.minimap2.03.log || exit 1

     awk 'BEGIN{t["none"]=1;}{if( $1 == "@SQ" ){if( $2 in t ){;}else{t[$2]=1;print $0;}}else{print $0;}}'\
         < $OUT_PREFIX.sam  >$OUT_PREFIX.fiter.sam || exit 1

    $SAMTOOL view -bo $OUT_PREFIX.bam  $OUT_PREFIX.fiter.sam -@ $CPU \
        >$OUT_PREFIX.samtool_01.log 2>&1 || exit 1

    $SAMTOOL sort $OUT_PREFIX.bam -o $OUT_PREFIX.sort.bam -@ $CPU \
        >$OUT_PREFIX.samtool_02.log 2>&1 || exit 1

    $SAMTOOL index $OUT_PREFIX.sort.bam -@ $CPU \
        >$OUT_PREFIX.samtool_03.log 2>&1 || exit 1

    $JAVA -Xmx$PILON_MEM -jar  $PILON --fix all \
        --genome $OUT_PREFIX.ont.$i.fasta --bam $OUT_PREFIX.sort.bam \
        --output $OUT_PREFIX.ont.$i.pilon --outdir ./ \
        --diploid  --threads $CPU >$OUT_PREFIX.pilon.log 2>$OUT_PREFIX.pilon.err || exit 1
done
if [[ $CHUNK_NUM != "1" ]] ; then 
    for ((i=0; i<CHUNK_NUM; i++))
    do
        cat $OUT_PREFIX.ont.$i.pilon.fasta >> $OUT_PREFIX.ont.pilon.fasta
    done
else
    ln -s $OUT_PREFIX.ont.0.pilon.fasta  $OUT_PREFIX.ont.pilon.fasta
fi
###########################################################
# Step 4 :
#   use ont-sub-fragments to fill gaps
###########################################################


$MINIMAP2  $MINIMAP2_PARAM  -t $CPU  \
    $OUT_PREFIX.ont.pilon.fasta $TMP_INPUT_SCAFTIG  \
    1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1

$BIN_DIR/TGSGapFiller --ont_reads_a $OUT_PREFIX.ont.pilon.fasta \
    --contig2ont_paf $OUT_PREFIX.fill.paf \
    --min_match=$MinMatch --min_idy=$MinIDY \
    --prefix $OUT_PREFIX 1>$OUT_PREFIX.fill.log  2>&1|| exit 1

$BIN_DIR/TGSSeqGen --prefix  $OUT_PREFIX 1>$OUT_PREFIX.i2s.log 2>&1  || exit 1
