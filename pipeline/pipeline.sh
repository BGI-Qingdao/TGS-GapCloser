#!/bin/bash

###########################################################
# Step 1 :
#   edit configuration .
###########################################################

# make sure output prefix != input prefix
OUT_PREFIX="test_03"
# this is a directory
BIN_DIR="/dellfsqd1/ST_OCEAN/ST_OCEAN/USRS/guolidong/software/source_dir/TGSGapFiller/bin"

# this is a file 
CONTIG="fake25K_2KN.contig"
# this is a file 
SCAFF_INFO="fake25K_2KN.scaff_infos"
# this is a file or ""
ONT_FA="/home/xumengyang/ONT/chr19_eva/Extract_Chr19_rel3.fa"
# this is a file or ""
ONT_FQ=""
# this is a file 
MINIMAP2="/home/softwares/minimap2-master/minimap2"

###########################################################
# Step 1.5 :
#   check 
###########################################################

if [[ "$OUT_PREFIX.contig" == $CONTIG  || "$OUT_PREFIX.scaff_infos" == $SCAFF_INFO ]] ; then 
    echo "ERROR OUTPUT PREFIX == INPUT PREFIX . exit ..."
    exit 1;
fi

###########################################################
# Step 2 :
#   use ont reads to fill gaps
###########################################################

if [[ $ONT_FA != "" ]] ; then 
    $MINIMAP2  -x ava-ont -t 30 $ONT_FA $CONTIG  --sam-hit-only \
        1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1

    $BIN_DIR/ONTGapFiller --ont_reads_a $ONT_FA \
        --contig2ont_paf $OUT_PREFIX.fill.paf --contig $CONTIG <$SCAFF_INFO \
        >$OUT_PREFIX.scaff_infos 2>$OUT_PREFIX.fill.log || exit 1
else
    $MINIMAP2  -x ava-ont -t 30 $ONT_FQ $CONTIG  --sam-hit-only \
        1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1

    $BIN_DIR/ONTGapFiller --ont_reads_q $ONT_FQ \
        --contig2ont_paf $OUT_PREFIX.fill.paf --contig $CONTIG <$SCAFF_INFO \
        >$OUT_PREFIX.scaff_infos 2>$OUT_PREFIX.fill.log || exit 1
fi

if [[ ! -f $OUT_PREFIX.contig ]] ; then
    ln -s $CONTIG $OUT_PREFIX.contig || exit 1
else
    echo "Warning : $OUT_PREFIX.contig already exist !!! use it anyway ."
fi

$BIN_DIR/ScaffInfo2Seq --prefix  $OUT_PREFIX --min_n 1 --min_c 0 \
    >$OUT_PREFIX.i2s.log 2>&1  || exit 1

