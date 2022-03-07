#!/bin/bash
#####################################################################
#
#   brief :  main pipeline script of TGSGapCloser.
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
echo "INFO  :   Run TGS-GapCloser from $HOOM_DIR ;"
echo "          Version : $VERSION ;"
echo "          Release time : $RELEASE_TIME ."
echo ""
#####################################################################
#
#   function
#
############################################################3########
function print_info_line()
{
    echo "              -   $1"
}
function print_info()
{
    echo "";
    echo "INFO  :   $1"
}
function print_fatal()
{
    echo "";
    echo "FATAL :   $1 ! exit ...";
    echo " try -h/--help for usage."
    exit 1 ;
}
function print_help()
{
    echo "Usage:"
    echo "      TGS-GapCloser.sh  --scaff SCAFF_FILE --reads TGS_READS_FILE --output OUT_PREFIX [options...]"
    echo "      required :"
    echo "          --scaff     <scaffold_file>      input scaffold file."
    echo "          --reads     <tgs_reads_file>     input TGS read file."
    echo "          --output    <output_prefix>      output prefix."
    echo "      part required :  "
    echo "          --ne                             do not correct errors. off by default."
    echo "          or"
    echo "          --racon     <racon>              installed racon path."
    echo "          or"
    echo "          --ngs       <ngs_reads>          ngs reads used for pilon"
    echo "          --pilon     <pilon>              installed pilon path."
    echo "          --samtools  <samtools>           installed samtools path."
    echo "          --java      <java>               installed java path."
    echo "      optional:"
    echo "          --minmap_arg <minmap2 args>      for example, --minmap_arg ' -x ava-ont'"
    echo "                                           arg must be wraped by ' '"
    echo "          --tgstype   <pb/ont>             TGS type. ont by default."
    echo "          --min_idy   <min_idy>            min_idy for filtering TGS reads ."
    echo "                                           0.3 for ont by default."
    echo "                                           0.2 for pb by default."
    echo "          --min_match <min_idy>            min match length for filtering TGS reads ."
    echo "                                           300bp for ont by default."
    echo "                                           200bp for pb by default."
    echo "          --thread    <t_num>              threads uesd. 16 by default."
    echo "          --chunk     <chunk_num>          split candidates into chunks to separately do error correction."
    echo "          --pilon_mem <t_num>              memory used for pilon. 300G for default."
    echo "          --p_round   <pilon_round>        iteration # of error corretion by pilon. 3 by default."
    echo "          --r_round   <racon_round>        iteration # of error corretion by racon. 1 by default."
    echo "          --g_check                        gapsize diff check. off by default."
    echo "          --min_nread <min_nread>          minimum number of reads that can cross this gap. 1 by default."
    echo "          --max_candidate <max_candidate>  maximum number of candidate alignment used for error correction and gapfilling. 10 by default"
}

function check_arg_null() {
    if [[ -z $2 ]] ; then 
        print_fatal "Arg $1 is not assigned !!!";
    fi
}

function check_file()
{
    if [[ ! -e $1 ]] ; then
        print_fatal "File $1 does not exist !!!"
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
MIN_MATCH_USER="no"
PILON_MEM="300G"
MINIMAP2_PARAM="  -I 20g  --split-prefix -x ava-ont "
USE_NEW_MINMAP2_ARG="no"
CHUNK_NUM=3
USE_RACON="yes"
PILON_ROUND=3
RACON_ROUND=1
G_CHECK=0
MIN_NREAD=1
MAX_CANDIDATE=10

print_info "Parsing args starting ..."
if [[ $# -lt 1 ]] ; then 
    print_help
    exit 1 ;
fi
ARGS=`getopt -o h  --long scaff:,reads:,output:,racon:,pilon:,ngs:,samtools:,java:,tgstype:,chunk:,thread:,minmap_arg:,min_idy:,min_match:,pilon_mem:,p_round:,r_round:,ne,min_nread:,max_candidate:,g_check  -- "$@"`
eval set -- "$ARGS"
while true; do
    case "$1" in
        -h|--help)
            shift;
            print_help;
            exit 1;
        ;;
        --minmap_arg)
            shift
            MINIMAP2_PARAM="$1"
            USE_NEW_MINMAP2_ARG='yes'
            shift;
            echo "          --minmap_arg $MINIMAP2_PARAM"
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
        --samtools)
            shift;
            SAMTOOL=$1
            shift;
            echo  "             --samtools  $SAMTOOL"
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
        --g_check)
            shift;
            G_CHECK=1
            echo  "             --g_check"
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
        --p_round)
            shift;
            PILON_ROUND=$1
            shift;
            echo  "             --p_round $PILON_ROUND"
        ;;
        --r_round)
            shift;
            RACON_ROUND=$1
            shift;
            echo  "             --r_round $RACON_ROUND"
        ;;
        --chunk)
            shift;
            CHUNK_NUM=$1
            shift;
            echo  "             --chunk $CHUNK_NUM"
        ;;
        --min_nread)
            shift;
            MIN_NREAD=$1
            shift;
            echo  "             --min_nread $MIN_NREAD"
        ;;
        --max_candidate)
            shift;
            MAX_CANDIDATE=$1
            shift;
            echo  "             --max_candidate $MAX_CANDIDATE"
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

pilon_mem_numer=${PILON_MEM%?}
samtools_mem_number=$((pilon_mem_numer / THREAD))
SAMTOOLS_MEM=${samtools_mem_number}"G"

print_info "Parsing args end ."
#####################################################################
#
#   check env
#
############################################################3########

# check binary
Candidate=$HOOM_DIR"/bin/TGSGapCandidate"
GapCloser=$HOOM_DIR"/bin/TGSGapCloser"
SeqGen=$HOOM_DIR"/bin/TGSSeqGen"
SeqSplit=$HOOM_DIR"/bin/TGSSeqSplit"
MiniMap2=$HOOM_DIR"/minimap2/minimap2"

print_info "Checking basic args & env ..."

check_file_exe $Candidate
check_file_exe $GapCloser
check_file_exe $SeqGen
check_file_exe $SeqSplit
check_file_exe $MiniMap2
# check input args.
check_arg_exist "input_scaff" $INPUT_SCAFF
check_arg_exist "reads"  $TGS_READS
check_arg_null "output" $OUT_PREFIX

if [[ $MIN_NREAD -lt 1 ]] ; then 
    echo "Error : min_nread < 1. exit ..."
    exit 1
fi

if [[ $NE == "no" ]] ; then
    if [[ $NGS_READS != "" ]] ; then 
        check_arg_exist "ngs" $NGS_READS
        check_arg_exist "pilon" $PILON
        check_arg_exe "samtools" $SAMTOOL
        check_arg_exe "java" $JAVA
        print_info_line "Doing error correction by pilon with ngs reads. "
        USE_RACON="no"
    else
        check_arg_exe "racon" $RACON
        print_info_line "Doing error correction by racon with tgs reads."
        USE_RACON="yes"
    fi
else 
    print_info_line "No error correction by --ne option"
fi
# pacbio special default values.
if [[ $TGS_TYPE == "pb" ]] ; then 
    if [[ $USE_NEW_MINMAP2_ARG != "yes" ]] ; then 
        MINIMAP2_PARAM="  -I 20g  --split-prefix  -x ava-pb "
    fi
    if [[ $MIN_IDY_USER == "no" ]] ; then
        MIN_IDY="0.2"
    fi
    if [[ $MIN_MATCH_USER == "no" ]] ; then
        MIN_MATCH="200"
    fi
fi
print_info_line "TGS reads type is $TGS_TYPE . MINIMAP2_PARAM is $MINIMAP2_PARAM  MIN_IDY is $MIN_IDY . MIN_MATCH is $MIN_MATCH ."

print_info "Checking basic args & env end."
#####################################################################
#
#   step 1 , split input scaffold
#
############################################################3########
TMP_INPUT_SCAFTIG=$OUT_PREFIX".contig"
print_info "Step 1 , run TGSSeqSplit to split scaffolds into scaftigs(contigs). "
if [[ ! -e 'done_step1_tag' ]] ; then
    $SeqSplit --input_scaff $INPUT_SCAFF \
        --prefix $OUT_PREFIX 2>$OUT_PREFIX.seq_split.log || exit 1 

    check_file $TMP_INPUT_SCAFTIG
    date >>'done_step1_tag'
else 
    echo 'skip step1 since done_step1_tag exists'
fi
print_info "Step 1 , done ."

#####################################################################
#
#   step 2 , choose candidate filling sequences .
#
############################################################3########
if [[ $NE == "yes" ]] ; then 
    print_info "Step 2 , skip TGSCandidate by --ne option."
else
    print_info "Step 2 , run TGSCandidate ... "
    if [[ ! -e 'done_step2.1_tag' ]] ; then
        # run minimap2
        print_info_line "2.1 , run minmap2 to map contigs against tgs reads."
        check_file $TMP_INPUT_SCAFTIG
        $MiniMap2  $MINIMAP2_PARAM  -t $THREAD  \
            $TGS_READS  $TMP_INPUT_SCAFTIG \
            1>$OUT_PREFIX.sub.paf 2>$OUT_PREFIX.minimap2.01.log || exit 1
        date >>'done_step2.1_tag'
    else
        echo 'skip step2.1 since done_step2.1_tag exists'
    fi

    if [[ ! -e 'done_step2.2_tag' ]] ; then
    # run candidate
        print_info_line "2.2 , run TGSGapCandidate to choose candidate seqs."

        TMP_INPUT_SCAFF_INFO=$OUT_PREFIX".orignial_scaff_infos"
        check_file $TMP_INPUT_SCAFF_INFO
        $Candidate --ont_reads_a $TGS_READS \
        --contig2ont_paf $OUT_PREFIX.sub.paf \
        --min_nread $MIN_NREAD \
        --candidate_max $MAX_CANDIDATE --candidate_shake_filter --candidate_merge \
        <$TMP_INPUT_SCAFF_INFO >$OUT_PREFIX.ont.fasta 2>$OUT_PREFIX.cand.log || exit 1
        # remove used paf here
        rm $OUT_PREFIX.sub.paf
        date >>'done_step2.2_tag'
    else
        echo 'skip step2.2 since done_step2.2_tag exists'
    fi

    if [[ ! -e 'done_step2.3_tag' ]] ; then
        # split candidate into chunk
        print_info_line "2.3 , break candidates into $CHUNK_NUM chunk(s)."
        if [[ $CHUNK_NUM != "1" ]] ; then
            for ((i=0; i<CHUNK_NUM; i++))
            do
                awk 'BEGIN{id=0}{if($1 ~ />/ ){id=id+1;} if(id % cov == the_i) {print $0;} }' cov=$CHUNK_NUM the_i=$i \
                    <$OUT_PREFIX.ont.fasta >$OUT_PREFIX.ont.0.$i.fasta || exit 1 ;
            done
        else
            ln -s $OUT_PREFIX.ont.fasta $OUT_PREFIX.ont.0.0.fasta
        fi
        date >>'done_step2.3_tag'
    else
        echo 'skip step2.3 since done_step2.3_tag exists'
    fi
    print_info "Step 2 , done ."
fi

#####################################################################
#
#   step 3 , error-correction .
#
############################################################3########
if [[ $NE == "yes" ]] ; then 
    print_info "Step 3 , skip error correction by --ne option."
else
    if [[ $USE_RACON == "yes" ]]  ; then
        print_info "Step 3.A , error correction by racon ...  "

        if [[ ! -e 'done_step_3.A.1_tag' ]] ; then 
            print_info_line "3.A.1 , racon each chunk ...  "
            prev_round=0;
            last_round=0;
            for ((round=0; round<RACON_ROUND ; round++))
            do
                if [[ $round != $last_round ]] ; then 
                    prev_round=$last_round
                    last_round=$round
                fi
                print_info_line "   -   iteration $round start ... "
                for ((i=0; i<CHUNK_NUM; i++))
                do
                    curr_tag="$round"".""$i"
                    print_info_line "   -   iteration $round chunk $i -  minimap2 indexing ... "
                    if [[ $round -gt 0 ]] ; then 
                        prev_tag="$prev_round"".""$i"
                        ln -s  $OUT_PREFIX.ont.$prev_tag.racon.fasta $OUT_PREFIX.ont.$curr_tag.fasta
                    fi
                    curr_tag="$round"".""$i"
                    print_info_line "   -   iteration $round chunk $i -  minimap2 ... "
                    $MiniMap2 -t $THREAD $MINIMAP2_PARAM $OUT_PREFIX.ont.$curr_tag.fasta $TGS_READS \
                        1>$OUT_PREFIX.$curr_tag.paf 2>$OUT_PREFIX.minimap2.03.$curr_tag.log || exit 1
                    print_info_line "   -   iteration $round chunk $i -  racon ... "
                    $RACON -t $THREAD $TGS_READS $OUT_PREFIX.$curr_tag.paf $OUT_PREFIX.ont.$curr_tag.fasta \
                        1>$OUT_PREFIX.ont.$curr_tag.racon.fasta 2>$OUT_PREFIX.$curr_tag.racon.log || exit 1

                    # remove used paf here
                    rm $OUT_PREFIX.$curr_tag.paf
                done
                print_info_line "   -   iteration $round end. "
            done
            date >>'done_step_3.A.1_tag'
        else 
            echo 'skip step3.A.1 since done_step3.A.1_tag exists'
        fi

        if [[ ! -e 'done_step_3.A.2_tag' ]] ; then
            print_info_line "3.A.2 , merge chunks ...  "
            if [[ $CHUNK_NUM != "1" ]] ; then 
                for ((i=0; i<CHUNK_NUM; i++))
                do
                    curr_tag="$last_round"".""$i"
                    cat $OUT_PREFIX.ont.$curr_tag.racon.fasta >> $OUT_PREFIX.ont.racon.fasta
                    cat $OUT_PREFIX.$curr_tag.racon.log >> $OUT_PREFIX.racon.log
                    rm -rf $OUT_PREFIX.ont.$curr_tag.racon.fasta
                    rm -rf $OUT_PREFIX.$curr_tag.racon.log
                done
            else
                curr_tag="$last_round"".0"
                mv $OUT_PREFIX.ont.$curr_tag.racon.fasta  $OUT_PREFIX.ont.racon.fasta
                mv $OUT_PREFIX.$curr_tag.racon.log  $OUT_PREFIX.racon.log
            fi
            date >>'done_step_3.A.2_tag'
        else
            echo 'skip step3.A.2 since done_step3.A.2_tag exists'
        fi
    else
        print_info "Step 3.B , error correction by pilon ...  "
        if [[ ! -e 'done_step_3.B.1_tag' ]] ; then
            print_info_line "3.B.1 ,  pilon each chunk ...  "
            prev_round=0;
            last_round=0;
            for ((round=0; round<PILON_ROUND ; round++))
            do
                print_info_line "   -   iteration $round start ... "
                if [[ $round != $last_round ]] ; then 
                    prev_round=$last_round
                    last_round=$round
                fi
                for ((i=0; i<CHUNK_NUM; i++))
                do
                    curr_tag="$round"".""$i"
                    print_info_line "   -   iteration $round chunk $i -  minimap2 indexing ... "
                    if [[ $round -gt 0 ]] ; then 
                        prev_tag="$prev_round"".""$i"
                        ln -s  $OUT_PREFIX.ont.$prev_tag.pilon.fasta $OUT_PREFIX.ont.$curr_tag.fasta
                    fi
                    $MiniMap2 -t $THREAD -d $OUT_PREFIX.mmi $OUT_PREFIX.ont.$curr_tag.fasta \
                        1>$OUT_PREFIX.minimap2.02.$curr_tag.log 2>&1 || exit 1
                    print_info_line "   -   iteration $round chunk $i -  minimap2 mapping ngs into tgs ... "
                    $MiniMap2 -t $THREAD -k14 -w5 -n2 -m20 -s 40 --sr --frag yes  \
                        --split-prefix=$OUT_PREFIX.$curr_tag \
                        -a $OUT_PREFIX.mmi  $NGS_READS \
                        1>$OUT_PREFIX.sam 2>$OUT_PREFIX.minimap2.03.$curr_tag.log || exit 1

                    print_info_line "   -   iteration $round chunk $i -  process required bam ... "
                    awk 'BEGIN{t["none"]=1;}{if( $1 == "@SQ" ){if( $2 in t ){;}else{t[$2]=1;print $0;}}else{print $0;}}'\
                        < $OUT_PREFIX.sam  >$OUT_PREFIX.fiter.sam || exit 1
                    # remove used sam 
                    rm $OUT_PREFIX.sam
                    $SAMTOOL view -bo $OUT_PREFIX.bam  $OUT_PREFIX.fiter.sam -@ $THREAD \
                            >$OUT_PREFIX.samtool_01.$curr_tag.log 2>&1 || exit 1
                    # remove used sam
                    rm $OUT_PREFIX.fiter.sam
                    $SAMTOOL sort -m $SAMTOOLS_MEM $OUT_PREFIX.bam -o $OUT_PREFIX.sort.bam -@ $THREAD \
                            >$OUT_PREFIX.samtool_02.$curr_tag.log 2>&1 || exit 1
                    # remove used bam
                    rm  $OUT_PREFIX.bam
                    $SAMTOOL index $OUT_PREFIX.sort.bam -@ $THREAD \
                            >$OUT_PREFIX.samtool_03.$curr_tag.log 2>&1 || exit 1
                    print_info_line  "   -   iteration $round chunk $i -  run pilon ... "
                    $JAVA -Xmx$PILON_MEM -jar  $PILON --fix all \
                        --genome $OUT_PREFIX.ont.$curr_tag.fasta --bam $OUT_PREFIX.sort.bam \
                        --output $OUT_PREFIX.ont.$curr_tag.pilon --outdir ./ \
                        --diploid  --threads $THREAD >$OUT_PREFIX.$curr_tag.pilon.log 2>$OUT_PREFIX.pilon.$curr_tag.err || exit 1
                    # remove used bam
                    rm $OUT_PREFIX.sort.bam

                done
                print_info_line "   -   iteration $round end. "
            done
            date >>'done_step_3.B.1_tag'
        else
            echo 'skip step3.B.1 since done_step3.B.1_tag exists'
        fi

        if [[ ! -e 'done_step_3.B.2_tag' ]] ; then
            print_info_line "3.B.2 , merge chunk results ... "
            if [[ $CHUNK_NUM != "1" ]] ; then 
                for ((i=0; i<CHUNK_NUM; i++))
                do
                    curr_tag="$last_round"".""$i"
                    cat $OUT_PREFIX.ont.$curr_tag.pilon.fasta >> $OUT_PREFIX.ont.pilon.fasta
                    cat $OUT_PREFIX.$curr_tag.pilon.log >> $OUT_PREFIX.pilon.log
                    rm -rf $OUT_PREFIX.ont.$curr_tag.pilon.fasta
                    rm -rf $OUT_PREFIX.$curr_tag.pilon.log
                done
            else
                curr_tag="$last_round"".0"
                mv $OUT_PREFIX.ont.$curr_tag.pilon.fasta  $OUT_PREFIX.ont.pilon.fasta
                mv $OUT_PREFIX.$curr_tag.pilon.log  $OUT_PREFIX.pilon.log
            fi
            date >>'done_step_3.B.2_tag'
        else
            echo 'skip step3.B.2 since done_step3.B.2_tag exists'
        fi
    fi
    print_info "Step 3 , done ."
fi

#####################################################################
#
# Step 4 :  gap-filling
#
#####################################################################

print_info "Step 4 , gap filling ... "
FINAL_READS=""
if [[ $NE == "yes" ]] ; then 
    print_info_line "Use $TGS_READS as final TGS READS input."
    FINAL_READS=$TGS_READS
else
    if [[ $USE_RACON == "yes" ]]  ; then  
        print_info_line "Use $OUT_PREFIX.ont.racon.fasta as final TGS READS input"
        FINAL_READS=$OUT_PREFIX.ont.racon.fasta
    else
        print_info_line "Use $OUT_PREFIX.ont.pilon.fasta as final TGS READS input"
        FINAL_READS=$OUT_PREFIX.ont.pilon.fasta
    fi
fi
check_file $FINAL_READS 
check_file $TMP_INPUT_SCAFTIG
if [[ ! -e 'done_step_4.1_tag' ]] ; then
    print_info_line "4,1 , mapping contigs against reads ... "
    $MiniMap2  $MINIMAP2_PARAM  -t $THREAD  $FINAL_READS $TMP_INPUT_SCAFTIG  \
            1>$OUT_PREFIX.fill.paf 2>$OUT_PREFIX.minimap2.04.log || exit 1
    date >>'done_step_4.1_tag'
else
    echo 'skip step4.1 since done_step4.1_tag exists'
fi
if [[ ! -e 'done_step_4.2_tag' ]] ; then
    print_info_line "4,2 , filling gaps ... "
    if [[ $G_CHECK == 0 ]] ; then
        $GapCloser --ont_reads_a $FINAL_READS --contig2ont_paf $OUT_PREFIX.fill.paf \
            --min_match=$MIN_MATCH --min_idy=$MIN_IDY \
            --prefix $OUT_PREFIX 1>$OUT_PREFIX.fill.log  2>&1|| exit 1
        # remove used paf 
        rm $OUT_PREFIX.fill.paf
    else
        $GapCloser --ont_reads_a $FINAL_READS --contig2ont_paf $OUT_PREFIX.fill.paf \
            --min_match=$MIN_MATCH --min_idy=$MIN_IDY \
            --prefix $OUT_PREFIX --use_gapsize_check  1>$OUT_PREFIX.fill.log  2>&1|| exit 1
        rm $OUT_PREFIX.fill.paf
    fi
    print_info_line "4,2 , gen final seqs ... "
    date >>'done_step_4.2_tag'
else
    echo 'skip step4.2 since done_step4.2_tag exists'
fi
$SeqGen --prefix  $OUT_PREFIX 1>$OUT_PREFIX.i2s.log 2>&1  || exit 1
print_info "Step 4 , done ."

#####################################################################
#
# ALL DONE
#
#####################################################################
print_info "ALL DONE !!! "
