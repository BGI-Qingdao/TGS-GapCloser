.PHONY: all clean bin 

CC 		   =	gcc
CXX 	   =	g++

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

jobs = TGSGapCloser TGSGapCandidate TGSSeqSplit TGSSeqGen

scripts=

all :  ${jobs} ${scripts}






TGSGapCloser_cpp = TGSGapCloser.cpp
TGSGapCloser_o = TGSGapCloser.o
TGSGapCloser : clean ${TGSGapCloser_o} ${source_o} ../bin
	${CXX} ${TGSGapCloser_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o TGSGapCloser
	mv $@ ../bin/




TGSGapCandidate_cpp = TGSGapCandidate.cpp
TGSGapCandidate_o = TGSGapCandidate.o
TGSGapCandidate : clean ${TGSGapCandidate_o} ${source_o} ../bin
	${CXX} ${TGSGapCandidate_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o TGSGapCandidate
	mv $@ ../bin/




TGSSeqSplit_cpp = TGSSeqSplit.cpp
TGSSeqSplit_o = TGSSeqSplit.o
TGSSeqSplit : clean ${TGSSeqSplit_o} ${source_o} ../bin
	${CXX} ${TGSSeqSplit_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o TGSSeqSplit
	mv $@ ../bin/




TGSSeqGen_cpp = TGSSeqGen.cpp
TGSSeqGen_o = TGSSeqGen.o
TGSSeqGen : clean ${TGSSeqGen_o} ${source_o} ../bin
	${CXX} ${TGSSeqGen_o} ${source_o} ${LD_FLAGS} ${DEUBG_CXX} -o TGSSeqGen
	mv $@ ../bin/


jobs_o=  ${TGSGapCloser_o} ${TGSGapCandidate_o} ${TGSSeqSplit_o} ${TGSSeqGen_o}

dirty	   =${jobs_o} ${jobs} ${source_o}

../bin:
	mkdir -p ../bin

clean:
	rm -rf ${dirty}

