.PHONY: all clean bin

CC 		   =	gcc
CXX 	   =	g++

CXXFLAGS   =	-std=c++11\
				-I../\
				-lz\
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

jobs = ORGC ONTGapCorrecter ONTGapFiller SplitScaffSeq SplitScaff2NamedContig ScaffInfo2Seq

all :  ${jobs}




ORGC_cpp = ORGC.cpp
ORGC_o = ORGC.o
ORGC : clean ${ORGC_o} ${source_o} ../bin
	${CXX} ${ORGC_o} ${source_o} ${DEUBG_CXX} -o ORGC
	mv $@ ../bin/




ONTGapCorrecter_cpp = ONTGapCorrecter.cpp
ONTGapCorrecter_o = ONTGapCorrecter.o
ONTGapCorrecter : clean ${ONTGapCorrecter_o} ${source_o} ../bin
	${CXX} ${ONTGapCorrecter_o} ${source_o} ${DEUBG_CXX} -o ONTGapCorrecter
	mv $@ ../bin/




ONTGapFiller_cpp = ONTGapFiller.cpp
ONTGapFiller_o = ONTGapFiller.o
ONTGapFiller : clean ${ONTGapFiller_o} ${source_o} ../bin
	${CXX} ${ONTGapFiller_o} ${source_o} ${DEUBG_CXX} -o ONTGapFiller
	mv $@ ../bin/




SplitScaffSeq_cpp = SplitScaffSeq.cpp
SplitScaffSeq_o = SplitScaffSeq.o
SplitScaffSeq : clean ${SplitScaffSeq_o} ${source_o} ../bin
	${CXX} ${SplitScaffSeq_o} ${source_o} ${DEUBG_CXX} -o SplitScaffSeq
	mv $@ ../bin/




SplitScaff2NamedContig_cpp = SplitScaff2NamedContig.cpp
SplitScaff2NamedContig_o = SplitScaff2NamedContig.o
SplitScaff2NamedContig : clean ${SplitScaff2NamedContig_o} ${source_o} ../bin
	${CXX} ${SplitScaff2NamedContig_o} ${source_o} ${DEUBG_CXX} -o SplitScaff2NamedContig
	mv $@ ../bin/




ScaffInfo2Seq_cpp = ScaffInfo2Seq.cpp
ScaffInfo2Seq_o = ScaffInfo2Seq.o
ScaffInfo2Seq : clean ${ScaffInfo2Seq_o} ${source_o} ../bin
	${CXX} ${ScaffInfo2Seq_o} ${source_o} ${DEUBG_CXX} -o ScaffInfo2Seq
	mv $@ ../bin/


jobs_o=  ${ORGC_o} ${ONTGapCorrecter_o} ${ONTGapFiller_o} ${SplitScaffSeq_o} ${SplitScaff2NamedContig_o} ${ScaffInfo2Seq_o}

dirty	   =${jobs_o} ${jobs} ${source_o}

../bin:
	mkdir -p ../bin

clean:
	rm -rf ${dirty}

