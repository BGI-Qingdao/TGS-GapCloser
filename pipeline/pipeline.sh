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

#######################################
# basic project settings.
#######################################

# prefix of output files.
OUT_PREFIX="gapfill_test"
# -t for minimap2
CPU=30
#######################################
# basic tools settings.
#######################################

# this is a directory
BIN_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/source_dir/TGSGapFiller/bin"
# this is a executive file 
MINIMAP2="/home/softwares/minimap2-master/minimap2"

###########################################################
# Step 1 :
#   preprocess input file
###########################################################

$BIN_DIR/TGSSeqSplit --input_scaff $INPUT_SCAFF_FA --prefix $OUT_PREFIX

###########################################################
# Step 2 :
#   map contig into ONT reads
###########################################################

TMP_INPUT_SCAFTIG=$OUT_PREFIX".contig"

$MINIMAP2  -x ava-ont -t $CPU $ONT_FA $TMP_INPUT_SCAFTIG --sam-hit-only \
    1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1

###########################################################
# Step 3 :
#   process gap filling .
###########################################################
$BIN_DIR/TGSGapFiller --ont_reads_a $ONT_FA  --contig2ont_paf $OUT_PREFIX.fill.paf \
    --prefix $OUT_PREFIX 1>$OUT_PREFIX.fill.log 2>&1 || exit 1

###########################################################
# Step 4 :
#   generate final sequences and changing details .
###########################################################
$BIN_DIR/TGSSeqGen --prefix $OUT_PREFIX 1>$OUT_PREFIX.i2s.log 2>&1  || exit 1
