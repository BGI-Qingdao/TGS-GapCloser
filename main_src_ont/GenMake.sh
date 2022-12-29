#!/bin/bash

scripts="\
"

apps="\
 tgsgapcloser\
 tgsgapcandidate\
 tgsseqsplit\
 tgsseqgen\
"

jobs_o=" "

function GenApp()
{
local AppName=$1
jobs_o="$jobs_o \${$AppName""_o}"


echo """

$AppName"_cpp 	=	"$AppName".cpp"
$AppName"_o   =	"$AppName".o"
$AppName" : clean \${"$AppName"_o} \${source_o} ../tgsgapcloserbin"
	\${CXX} \${$AppName"_o} \${source_o} \${LD_FLAGS} \${DEUBG_CXX} -o "$AppName
	mv \$@ ../tgsgapcloserbin/

""">>Makefile

}

function GenScript()
{
local SS=$1
echo """
$SS:
	cp $SS ../bin/
""">>Makefile	
}

echo ".PHONY: all clean bin ${scripts}"  >Makefile
echo """
CC 		   =	gcc
CXX 	   =	g++

CXXFLAGS   =	-std=c++11\\
				-I../\\


LD_FLAGS   =    -L../minimap2\\
				-lz\\
				-lminimap2\\
				-lpthread\\


DEUBG_CXX  =	\${CXXFLAGS} -g
RELEASE_CXX=	\${CXXFLAGS}

source_cpp =	../common/files/file_reader.cpp \\
		   		../biocommon/sam_bam/sam_parser.cpp\\
		   		../biocommon/pair/pair_sam_parser.cpp\\
				../biocommon/fasta/fasta.cpp\\
				../biocommon/fastq/fastq.cpp\\
				../biocommon/seq/seq.cpp\\
				../biocommon/paf/PAF.cpp\\
                ../biocommon/align_common/align_result.cpp\\
		   		../common/files/file_writer.cpp \\
		   		../common/files/gzstream.cpp \\
				../common/log/log.cpp\\
				../common/log/logfilter.cpp\\
				../common/time/timetools.cpp\\
				../common/string/stringtools.cpp\\
                ../appcommon/contigPairInfo.cpp\\
                ../appcommon/ScaffInfo.cpp\\
                ../appcommon/ONT2Gap.cpp\\
				../common/args/argsparser.cpp\\


source_o		= \${source_cpp:%.cpp=%.o}

.cpp.o:
	\${CXX} \${DEUBG_CXX} -c \$< -o \$@

jobs =$apps

scripts=$scripts

all :  \${jobs} \${scripts}



""" >>Makefile
for x in $scripts
do
    GenScript $x
done
for x in $apps
do
    GenApp $x
done

echo "jobs_o=$jobs_o">>Makefile
echo """
dirty	   =\${jobs_o} \${jobs} \${source_o}

../tgsgapcloserbin:
	mkdir -p ../tgsgapcloserbin

clean:
	rm -rf \${dirty}
""" >>Makefile 
