#!/bin/bash
#####################################################################
#
#   version 
#
############################################################3########
VERSION="1.1.1"
RELEASE_TIME="2019-12-31"
HOOM_DIR=`dirname $0`
echo "INFO  :   Run TGSGapFiller from $HOOM_DIR ;"
echo "          Version : $VERSION ;"
echo "          Release time : $RELEASE_TIME ."
echo ""
#####################################################################
#
#   function
#
############################################################3########
function print_help()
{
    echo "Usage:"
    echo "      TGSGapFiller --scaff SCAFF_FILE --reads TGS_READS_FILE --output OUT_PREFIX [options...]"
    echo "      required :"
    echo "          --scaff     <scaffold_file>      the input scaffold file."
    echo "          --reads     <tgs_reads_file>     the input TGS reads file."
    echo "          --output    <output_prefix>      the output prefix."
    echo "      part required :  "
    echo "          --ne                             do not error correct. error correct by default."
    echo "          or"
    echo "          --racon     <racon_path>         the installed racon."
    echo "          or"
    echo "          --pilon     <pilon_path>         the installed pilon."
    echo "          --ngs       <ngs_reads>          the ngs reeds used for pilon"
    echo "      optional:"
    echo "          --tgstype   <pb/ont>             TGS type . ont by default."
    echo "          --min_idy   <min_idy>            min_idy for filter reads ."
    echo "                                           0.3 for ont by default."
    echo "                                           0.2 for pb by default."
    echo "          --min_match <min_idy>            min match length for filter reads ."
    echo "                                           300bp for ont by default."
    echo "                                           200bp for pb by default."
    echo "          --thread    <t_num>              thread uesd . 16 by default."
    echo "          --pilon_mem <t_num>              memory used for pilon , 300G for default."
}

function check_file()
{
    if [[ ! -e $1 ]] ; then
        echo "File $1 is not exist !! exit ..."
        exit 1;
    fi
}

#####################################################################
#
#   check args
#
############################################################3########
INPUT_SCAFF=""
TGS_READS=""
OUT_PREFIX=""

RACON=""
PILON=""
NGS_READS=""

NE="no"
TGS_TYPE="ont"
THREAD=16
MIN_IDY="0.3"
MIN_IDY_USER="no"
MIN_MATCH="300"
MIN_MATCH_USER="false"
PILON_MEM="300G"
MINIMAP2_PARAM=" -x ava-ont "

ARGS=`getopt -o h  --long scaff:,reads:,output:,racon:,pilon:,ngs:,tgstype:,thread:,min_idy:,min_match:,pilon_mem:,ne  -- "$@"`
eval set -- "$ARGS"
echo "INFO  :   Parsing args starting ..."
echo ""
while true; do
    case "$1" in
        -h|--help)
            shift;
            print_help;
            exit;
        ;;
        --scaff)
            shift;
            INPUT_SCAFF=$1
            shift;
            echo  "             --scaff $INPUT_SCAFF"
        ;;
        --reads)
            shift;
            TGS_READS=$1
            shift;
            echo  "             --reads $TGS_READS"
        ;;
        --output)
            shift;
            OUT_PREFIX=$1
            shift;
            echo  "             --output $OUT_PREFIX"
        ;;
        --racon)
            shift;
            RACON=$1
            shift;
            echo  "             --racon  $RACON"
        ;;
        --pilon)
            shift;
            PILON=$1
            shift;
            echo  "             --pilon  $PILON"
        ;;
        --ngs)
            shift;
            NGS_READS=$1
            shift;
            echo  "             --ngs    $NGS_READS"
        ;;
        --tgstype)
            shift;
            TGS_TYPE=$1
            shift;
            echo  "             --tgstype $TGS_TYPE"
        ;;
        --min_idy)
            shift;
            MIN_IDY=$1
            MIN_IDY_USER="yes"
            shift;
            echo  "             --min_idy $MIN_IDY"
        ;;
        --min_match)
            shift;
            MIN_MATCH=$1
            MIN_MATCH_USER="yes"
            shift;
            echo  "             --min_match $MIN_MATCH"
        ;;
        --thread)
            shift;
            THREAD=$1
            shift;
            echo  "             --thread $THREAD"
        ;;
        --pilon_mem)
            shift;
            PILON_MEM=$1
            shift;
            echo  "             --pilon_mem $PILON_MEM"
        ;;
        --ne)
            shift;
            NE="yes"
            echo  "             --ne "
        ;;
        --)
            shift;
            break;
        ;;
    esac
done
echo ""
echo "INFO  :   Parsing args end ."
echo ""
#####################################################################
#
#   check env
#
############################################################3########

# check binary
Candidate=$HOOM_DIR"/bin/TGSGapCandidate"
GapFiller=$HOOM_DIR"/bin/TGSGapFiller"
SeqGen=$HOOM_DIR"/bin/TGSSeqGen"
SeqSplit=$HOOM_DIR"/bin/TGSSeqSplit"
check_file $Candidate
check_file $GapFiller
check_file $SeqGen
check_file $SeqSplit
# check input files.
check_file $INPUT_SCAFF
check_file $TGS_READS
if [[ $NE == "no" ]] ; then
    if [[ $NGS_READS != "" ]] ; then 
        check_file $NGS_READS
        check_file $PILON
    else
        check_file $RACON
    fi
fi
# pacbio special default values.
if [[ $TGS_TYPE == "pb" ]] ; then 
    MINIMAP2_PARAM=" -x ava-pb "
    if [[ $MIN_IDY_USER == "no" ]] ; then
        MIN_IDY="0.2"
    fi
    if [[ $MIN_MATCH_USER == "no" ]] ; then
        MIN_IDY="200"
    fi
fi

#####################################################################
#
#   step 1 , split input scaffold
#
############################################################3########

