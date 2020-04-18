.PHONY: all clean tgsgapcloserbin 

CC ?=	gcc
CXX ?=	g++

CXXFLAGS   =	-std=c++11\
				-I../\


LD_FLAGS   =    -L../minimap2\
				-lz\
				-lminimap2\
				-lpthread\


DEUBG_CXX  =	${CXXFLAGS} -g
RELEASE_CXX=	${CXXFLAGS}

source_cpp =	../common/files/file_reader.cpp \
		   		../biocommon/sam_bam/sam_parser.cpp\
		   		../biocommon/pair/pair_sam_parser.cpp\
				../biocommon/fasta/fasta.cpp\
				../biocommon/fastq/fastq.cpp\
				../biocommon/seq/seq.cpp\
				../biocommon/paf/PAF.cpp\
                ../biocommon/align_common/align_result.cpp\
		   		../common/files/file_writer.cpp \
		   		../common/files/gzstream.cpp \
				../common/log/log.cpp\
				../common/log/logfilter.cpp\
				../common/time/timetools.cpp\
				../common/string/stringtools.cpp\
                ../appcommon/contigPairInfo.cpp\
                ../appcommon/ScaffInfo.cpp\
                ../appcommon/ONT2Gap.cpp\
				../common/args/argsparser.cpp\


source_o		= ${source_cpp:%.cpp=%.o}

.cpp.o:
	${CXX} ${DEUBG_CXX} -c $< -o $@

jobs = tgsgapcloser tgsgapcandidate tgsseqsplit tgsseqgen

scripts=

all :  ${jobs} ${scripts}






tgsgapcloser_cpp = tgsgapcloser.cpp
tgsgapcloser_o = tgsgapcloser.o
tgsgapcloser : clean ${tgsgapcloser_o} ${source_o} ../tgsgapcloserbin
	${CXX} ${tgsgapcloser_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o tgsgapcloser
	mv $@ ../tgsgapcloserbin/




tgsgapcandidate_cpp = tgsgapcandidate.cpp
tgsgapcandidate_o = tgsgapcandidate.o
tgsgapcandidate : clean ${tgsgapcandidate_o} ${source_o} ../tgsgapcloserbin
	${CXX} ${tgsgapcandidate_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o tgsgapcandidate
	mv $@ ../tgsgapcloserbin/




tgsseqsplit_cpp = tgsseqsplit.cpp
tgsseqsplit_o = tgsseqsplit.o
tgsseqsplit : clean ${tgsseqsplit_o} ${source_o} ../tgsgapcloserbin
	${CXX} ${tgsseqsplit_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o tgsseqsplit
	mv $@ ../tgsgapcloserbin/




tgsseqgen_cpp = tgsseqgen.cpp
tgsseqgen_o = tgsseqgen.o
tgsseqgen : clean ${tgsseqgen_o} ${source_o} ../tgsgapcloserbin
	${CXX} ${tgsseqgen_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o tgsseqgen
	mv $@ ../tgsgapcloserbin/


jobs_o=  ${tgsgapcloser_o} ${tgsgapcandidate_o} ${tgsseqsplit_o} ${tgsseqgen_o}

dirty	   =${jobs_o} ${jobs} ${source_o}

../tgsgapcloserbin:
	mkdir -p ../tgsgapcloserbin

clean:
	rm -rf ${dirty}

