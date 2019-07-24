.PHONY: all clean

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

test_cpp   =	test.cpp\
				../biocommon/fasta/fasta_test.cpp\
				../biocommon/seq/seq_test.cpp\
				../algorithm/align/SmithWaterman_test.cpp\
				../algorithm/interval/Interval_test.cpp\
				../algorithm/distribution/distribution_test.cpp\
				../algorithm/linear_fitting/Minimum_multiplication_test.cpp\
				../algorithm/graph/GraphTrunk_test.cpp\
				../algorithm/graph/GraphTipRemove_test.cpp\
				../algorithm/graph/MinTree_test.cpp\
				../algorithm/graph/SPFSearch_test.cpp\
				../algorithm/graph/GraphTest.cpp\
				../algorithm/graph/GraphBasic_test.cpp\
				../algorithm/bi_list/bi_list_test.cpp\
				../algorithm/fibheap/fib_heap_test.cpp\
				../algorithm/collection/collection_test.cpp\
				../algorithm/incr_array/incr_array_test.cpp\
				../common/time/timetools_test.cpp\
				../common/string/stringtools_test.cpp\
				../common/args/argsparser_test.cpp\
				../common/files/file_test.cpp\
				../biocommon/sam_bam/sam_parser_test.cpp\
				../appcommon/SubSets_test.cpp\


source_o		= ${source_cpp:%.cpp=%.o}

test_o 	   =	${test_cpp:%.cpp=%.o} ${source_cpp:%.cpp=%.o}

.cpp.o:
	${CXX} ${DEUBG_CXX} -c $< -o $@

all : test 

test: clean ${test_o} ${source_h}
	${CXX} ${DEUBG_CXX} ${test_o} -o test

dirty	   =	${test_o}  test

clean:
	rm -rf ${dirty}
