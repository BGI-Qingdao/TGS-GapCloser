#!/bin/bash

##################################################
#
# PLEASE MODIFY BELOW CONFIG .
#
##################################################

OUT_PREFIX="test"
# you must give a ont read. a fa or a fq
INPUT_ONT_READ_FA="/nascngb/gccnt/wangou/assemblerdev/xumengyang/data/8X_rel4_60X_stLFR_minimap2_racon_round1.fa"
#INPUT_ONT_READ_FQ=""

# USE_SCAFF_SEQ = "yes" or "no".
#   * yes use INPUT_SCAFFOLD_SEQ
#   * no  use INPUT_CONTIG && INPUT_SCAFF_INFOS
USE_SCAFF_SEQ="yes"
INPUT_SCAFFOLD_SEQ="/nascngb/gccnt/wangou/assemblerdev/xumengyang/data/human_all_pseudohap.fasta"
#INPUT_CONTIG=""
#INPUT_SCAFF_INFOS=""

# USE_PA F= "yes" or "no".
#   * yes use INPUT_PAF
#   * no  use MINMAP to align scaftig 2 ont reads.
USE_PAF="no"
#INPUT_PAF=""

BIN_DIR="/hwfssz1/ST_OCEAN/USER/guolidong/stLFR/debug/stLFR_Assembler/src/bin/"
MINMAP="/ldfssz1/ST_OCEAN/USER/xumengyang/software/minimap2/minimap2"
CPU=30


USE_ORDER="no"
USE_ORIENTATION="no"
USE_GAP_FILLER="yes"
USE_GAP_CORRETOR="no"
GAP_MODE=1  # 1 for shortest ; 3 for median ; 2 for random

######################################################
#
# DO NOT MODIFY BELOW CODE UNLESS YOU KNOW THE LOGIC .
#
######################################################

OUT_CONTIG=$OUT_PREFIX".contig"
OUT_PAF=$OUT_PREFIX".paf"
OUT_SCAF_INFOS=$OUT_PREFIX".scaff_infos"
OUT_SCAF_INFOS_0=$OUT_PREFIX".scaff_infos_0"
OUT_SCAF_INFOS_1=$OUT_PREFIX".scaff_infos_1"
OUT_SCAF_INFOS_2=$OUT_PREFIX".scaff_infos_2"
OUT_SCAF_INFOS_3=$OUT_PREFIX".scaff_infos_3"
OUT_SCAF_INFOS_4=$OUT_PREFIX".scaff_infos_4"

## prepare check ...
# check ont read
if [[  $INPUT_ONT_READ_FA == "" && $INPUT_ONT_READ_FQ == "" ]] ; then
    echo "Please give a ont read. exit ..."
    exit
fi

if [[  $INPUT_ONT_READ_FA == ""  ]] ; then
    if [[ ! -e $INPUT_ONT_READ_FQ ]] ; then
        echo "1 $INPUT_ONT_READ_FQ is not exsist exit ..."
        exit
    fi
    FQ="yes"
else
    if [[ ! -e $INPUT_ONT_READ_FA ]] ; then
        echo " 2 $INPUT_ONT_READ_FA is not exsist exit ..."
        exit
    fi
    FQ="no"
fi

# check scaff*
if [[ $USE_SCAFF_SEQ == "yes" ]] ; then
    if [[ ! -e $INPUT_SCAFFOLD_SEQ ]] ; then
        echo "3 $INPUT_SCAFFOLD_SEQ is not exsist exit ..."
        exit
    fi
else
    if [[ ! -e $INPUT_CONTIG || ! -e $INPUT_SCAFF_INFOS ]] ; then
        echo "4 $INPUT_CONTIG or $INPUT_SCAFF_INFOS is not exsist exit ..."
        exit
    fi
fi

# check paf or minimap2
if [[ $USE_PAF == "yes" ]] ; then
    if [[ ! -e $INPUT_PAF ]] ; then
        echo "5 $INPUT_PAF is not exsist exit ..."
        exit
    fi
else
    if [[ ! -x $MINMAP ]] ; then
        echo "6 $MINMAP is not exsist or executable exit ..."
        exit
    fi
fi

## step 0 . split seqs if needed
if [[ $USE_SCAFF_SEQ == "yes" ]] ; then
    $BIN_DIR/SplitScaffSeq <$INPUT_SCAFFOLD_SEQ  >$OUT_CONTIG 2>$OUT_SCAF_INFOS
else
    ln -s $INPUT_CONTIG $OUT_CONTIG
    ln -s $INPUT_SCAFF_INFOS $OUT_SCAF_INFOS
fi

## step 1 . aligned if needed
if [[ $USE_PAF == "yes" ]] ; then
    ln -s $INPUT_PAF $OUT_PAF
else
    if [[ $FQ == "yes" ]] ; then
         $MINMAP -x ava-ont -t $CPU $INPUT_ONT_READ_FQ $OUT_CONTIG  1>$OUT_PAF 2>log_minimap2
    else
         $MINMAP -x ava-ont -t $CPU $INPUT_ONT_READ_FA $OUT_CONTIG  1>$OUT_PAF 2>log_minimap2
    fi
fi

## step 2. ONT process

mv $OUT_SCAF_INFOS  $OUT_SCAF_INFOS_0
ln -sf $OUT_SCAF_INFOS_0 $OUT_SCAF_INFOS
if [[ $USE_ORDER == "yes " ]] ; then
    echo " Order is not valid now . continue ..."
fi

if [[ $USE_ORIENTATION == "yes " ]] ; then
    echo " Order is not valid now . continue ..."
fi

if [[ $USE_GAP_CORRETOR  == "yes" ]] ; then
    $BIN_DIR/ONTGapCorrecter --contig2ont_paf $OUT_PAF --force --work_mode $GAP_MODE <$OUT_SCAF_INFOS >$OUT_SCAF_INFOS_3
    rm -rf $OUT_SCAF_INFOS
    ln -s $OUT_SCAF_INFOS_3 $OUT_SCAF_INFOS
fi

if [[ $USE_GAP_FILLER  == "yes" ]] ; then
    if [[ $FQ == "yes" ]] ; then
        $BIN_DIR/ONTGapFiller --ont_reads_q $INPUT_ONT_READ_FQ --contig2ont_paf $OUT_PAF --force --work_mode $GAP_MODE <$OUT_SCAF_INFOS >$OUT_SCAF_INFOS_4 2>log_filler
    else
        $BIN_DIR/ONTGapFiller --ont_reads_a $INPUT_ONT_READ_FA --contig2ont_paf $OUT_PAF --force --work_mode $GAP_MODE <$OUT_SCAF_INFOS >$OUT_SCAF_INFOS_4 2>log_filler
    fi
    rm -rf $OUT_SCAF_INFOS
    ln -s $OUT_SCAF_INFOS_4 $OUT_SCAF_INFOS
fi

## step 3. re-generate seq

$BIN_DIR/ScaffInfo2Seq --prefix  $OUT_PREFIX --min_n 1 --min_c 1

