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

#######################################
# basic 3rd part tools settings.
#######################################

# this is a directory
BIN_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/source_dir/TGSGapFiller/bin"
# this is a executive file 
MINIMAP2="/home/softwares/minimap2-master/minimap2"

###########################################################
# Step 1 :
#   preprocess input file
###########################################################

TMP_INPUT_SCAFTIG=$OUT_PREFIX".contig"
TMP_INPUT_SCAFF_INFO=$OUT_PREFIX".input.scaff_infos"

$BIN_DIR/SplitScaffSeq <$INPUT_SCAFF_FA >$TMP_INPUT_SCAFTIG 2>$TMP_INPUT_SCAFF_INFO

###########################################################
# Step 2 :
#   use ont-sub-fragments to fill gaps
###########################################################


$MINIMAP2  -x ava-ont -t 30 $ONT_FA $TMP_INPUT_SCAFTIG --sam-hit-only \
    1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1

$BIN_DIR/ONTGapFiller --ont_reads_a $ONT_FA \
    --contig2ont_paf $OUT_PREFIX.fill.paf --contig $TMP_INPUT_SCAFTIG <$TMP_INPUT_SCAFF_INFO\
    >$OUT_PREFIX.scaff_infos 2>$OUT_PREFIX.fill.log || exit 1

$BIN_DIR/ScaffInfo2Seq --prefix  $OUT_PREFIX --min_n 1 --min_c 0 \
    2>$OUT_PREFIX.i2s.log 2>&1  || exit 1
