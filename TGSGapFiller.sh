#!/bin/bash
#####################################################################
#
#   brief :  main pipeline script of TGSGapFiller.
#   usage :  use -h/--help for details.
#   autor :  xumengyang@genomics.cn && guolidong@genomics.cn
#
############################################################3########

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
function print_info()
{
    echo "";
    echo "INFO  :   $1"
}
function print_fatal()
{
    echo "";
    echo "FATAL :   $1 ! exit ..."
    exit 1 ;
}
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
    echo "          --racon     <racon>              the installed racon."
    echo "          or"
    echo "          --ngs       <ngs_reads>          the ngs reads used for pilon"
    echo "          --pilon     <pilon>              the installed pilon."
    echo "          --samtool   <samtool>            the installed samtool."
    echo "          --java      <java>               the installed java."
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
    echo "          --chunk     <chunk_num>          split candidate into chunks to error-correct separately."
}

function check_arg_null() {
    if [[ -z $2 ]] ; then 
        print_fatal "Arg $1 is not assigned !!!";
    fi
}

function check_file()
{
    if [[ ! -e $1 ]] ; then
        print_fatal "File $1 is not exist !!!"
    fi
}

function check_file_exe()
{
    check_file $1
    if [[ ! -x $1 ]] ; then
        print_fatal "File $1 is not executable !!!"
    fi
}
function check_arg_exist()
{
    check_arg_null $1 $2
    check_file $2
}

function check_arg_exe()
{
    check_arg_null $1 $2
    check_file_exe $2
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
JAVA=""
SAMTOOL=""

NE="no"
TGS_TYPE="ont"
THREAD=16
MIN_IDY="0.3"
MIN_IDY_USER="no"
MIN_MATCH="300"
MIN_MATCH_USER="false"
PILON_MEM="300G"
MINIMAP2_PARAM=" -x ava-ont "
CHUNK_NUM=3
USE_RACON="yes"

print_info "Parsing args starting ..."
echo ""

ARGS=`getopt -o h  --long scaff:,reads:,output:,racon:,pilon:,ngs:,samtool:,java:,tgstype:,thread:,min_idy:,min_match:,pilon_mem:,ne  -- "$@"`
eval set -- "$ARGS"
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
        --samtool)
            shift;
            SAMTOOL=$1
            shift;
            echo  "             --samtool  $SAMTOOL"
        ;;
        --java)
            shift;
            JAVA=$1
            shift;
            echo  "             --java  $JAVA"
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
        --chunk)
            shift;
            CHUNK_NUM=$1
            shift;
            echo  "             --chunk $CHUNK_NUM"
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

print_info "Parsing args end ."
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
MiniMap2=$HOOM_DIR"/minimap2/minimap2"

print_info "Checking basic args & env ..."

check_file_exe $Candidate
check_file_exe $GapFiller
check_file_exe $SeqGen
check_file_exe $SeqSplit
check_file_exe $MiniMap2
# check input args.
check_arg_exist "input_scaff" $INPUT_SCAFF
check_arg_exist "reads"  $TGS_READS
check_arg_null "output" $OUT_PREFIX


if [[ $NE == "no" ]] ; then
    if [[ $NGS_READS != "" ]] ; then 
        check_arg_exist "ngs" $NGS_READS
        check_arg_exe "pilon" $PILON
        check_arg_exe "samtool" $SAMTOOL
        check_arg_exe "java" $JAVA
        print_info "    -   Will do error-correcting by pilon with ngs-reads. "
        USE_RACON="no"
    else
        check_arg_exe "racon" $RACON
        print_info "    -   Will do error-correcting by racon."
        USE_RACON="yes"
    fi
else 
    print_info "    -   Will not do error-correcting by --ne option"
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
print_info "    -   TGS reads type is $TGS_TYPE . MIN_IDY is $MIN_IDY . MIN_MATCH is $MIN_MATCH ."

print_info "Checking basic args & env end."
#####################################################################
#
#   step 1 , split input scaffold
#
############################################################3########
print_info "step 1 , run TGSSeqSplit to split scaffolds into contigs. "
$SeqSplit --input_scaff $INPUT_SCAFF \
    --prefix $OUT_PREFIX 2>$OUT_PREFIX.seq_split.log || exit 1 

print_info "step 1 , done ."

#####################################################################
#
#   step 2 , choose candidate filling sequences .
#
############################################################3########

if [[ $NE == "yes" ]] ; then 
    print_info "step 2 , skip TGSCandidate by --ne option."
else
    print_info "step 2 , run TGSCandidate ... "
    # run minimap2
    print_info "     -   2.1 , run minmap2 to map contig into tgs-reads."
    TMP_INPUT_SCAFTIG=$OUT_PREFIX".contig"
    check_file $TMP_INPUT_SCAFTIG
    $MiniMap2  $MINIMAP2_PARAM  -t $THREAD  \
        $TGS_READS  $TMP_INPUT_SCAFTIG \
        1>$OUT_PREFIX.sub.paf 2>$OUT_PREFIX.minimap2.01.log || exit 1
    # run candidate
    print_info "     -   2.2 , run TGSGapCandidate to choose candidate region seqs."

    TMP_INPUT_SCAFF_INFO=$OUT_PREFIX".orignial_scaff_infos"
    check_file $TMP_INPUT_SCAFF_INFO
    $Candidate --ont_reads_a $TGS_READS \
        --contig2ont_paf $OUT_PREFIX.sub.paf \
        --candidate_max 10 --candidate_shake_filter --candidate_merge \
        <$TMP_INPUT_SCAFF_INFO >$OUT_PREFIX.ont.fasta 2>$OUT_PREFIX.cand.log || exit 1
    # split candidate into chunk
    print_info "     -   2.3 , break candidate into $CHUNK_NUM chunk(s)."
    if [[ $CHUNK_NUM != "1" ]] ; then
        for ((i=0; i<CHUNK_NUM; i++))
        do
            awk 'BEGIN{id=0}{if($1 ~ />/ ){id=id+1;} if(id % cov == the_i) {print $0;} }' cov=$CHUNK_NUM the_i=$i \
                <$OUT_PREFIX.ont.fasta >$OUT_PREFIX.ont.$i.fasta || exit 1 ;
        done
    else
        ln -s $OUT_PREFIX.ont.fasta $OUT_PREFIX.ont.0.fasta
    fi
    print_info "step 2 , done ."
fi

#####################################################################
#
#   step 3 , error-correction .
#
############################################################3########
if [[ $NE == "yes" ]] ; then 
    print_info "step 3 , skip error-correction by --ne option."
else
    if [[ $USE_RACON == "yes"]]  ; then
        print_info "step 3 , error-correction by racon ...  "
        print_info "       -    racon each chunk ...  "
        $MiniMap2 -t $THREAD $MINIMAP2_PARAM $OUT_PREFIX.ont.$i.fasta $TGS_FA 1>$OUT_PREFIX.$i.paf 2>$OUT_PREFIX.minimap2.03.log
        $RACON -t $CPU $TGS_FA $OUT_PREFIX.$i.paf $OUT_PREFIX.ont.$i.fasta 1>$OUT_PREFIX.ont.$i.raconr1.fasta 2>$OUT_PREFIX.$i.racon.log
    else
        print_info "step 3 , error-correction by pilon ...  "
        print_info "       -    pilon each chunk ...  "
        for ((i=0; i<CHUNK_NUM; i++))
        do
            print_info "            -   -   chunk $i -  minimap2 indexing ... "
            $MiniMap2 -t $THREAD -d $OUT_PREFIX.mmi $OUT_PREFIX.ont.$i.fasta \
                1>$OUT_PREFIX.minimap2.02.log 2>&1 || exit 1
            print_info "            -   -   chunk $i -  minimap2 mapping ngs into tgs ... "
            $MiniMap2-t $THREAD -k14 -w5 -n2 -m20 -s 40 --sr --frag yes  \
                --split-prefix=$OUT_PREFIX.$i \
                -a $OUT_PREFIX.mmi  $READ12  \
                1>$OUT_PREFIX.sam 2>$OUT_PREFIX.minimap2.03.log || exit 1

            print_info "            -   -   chunk $i -  process required bam ... "
            awk 'BEGIN{t["none"]=1;}{if( $1 == "@SQ" ){if( $2 in t ){;}else{t[$2]=1;print $0;}}else{print $0;}}'\
                < $OUT_PREFIX.sam  >$OUT_PREFIX.fiter.sam || exit 1
            $SAMTOOL view -bo $OUT_PREFIX.bam  $OUT_PREFIX.fiter.sam -@ $THREAD \
                    >$OUT_PREFIX.samtool_01.log 2>&1 || exit 1
            $SAMTOOL sort -m 8G $OUT_PREFIX.bam -o $OUT_PREFIX.sort.bam -@ $THREAD \
                    >$OUT_PREFIX.samtool_02.log 2>&1 || exit 1
            $SAMTOOL index $OUT_PREFIX.sort.bam -@ $THREAD \
                    >$OUT_PREFIX.samtool_03.log 2>&1 || exit 1
            print_info  "           -   -   chunk $i -  run pilon ... "
            $JAVA -Xmx$PILON_MEM -jar  $PILON --fix all \
                --genome $OUT_PREFIX.ont.$i.fasta --bam $OUT_PREFIX.sort.bam \
                --output $OUT_PREFIX.ont.$i.pilon --outdir ./ \
                --diploid  --threads $THREAD >$OUT_PREFIX.$i.pilon.log 2>$OUT_PREFIX.pilon.err || exit 1
        done
        print_info "       -    merge chunk results... "
        if [[ $CHUNK_NUM != "1" ]] ; then 
            for ((i=0; i<CHUNK_NUM; i++))
            do
                cat $OUT_PREFIX.ont.$i.pilon.fasta >> $OUT_PREFIX.ont.pilon.fasta
                cat $OUT_PREFIX.$i.pilon.log >> $OUT_PREFIX.pilon.log
                rm -rf $OUT_PREFIX.ont.$i.pilon.fasta
                rm -rf $OUT_PREFIX.$i.pilon.log
            done
        else
            mv $OUT_PREFIX.ont.0.pilon.fasta  $OUT_PREFIX.ont.pilon.fasta
            mv $OUT_PREFIX.0.pilon.log  $OUT_PREFIX.pilon.log
        fi
    fi
    print_info "step 3 , done ."
fi
