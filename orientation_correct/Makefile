.PHONY: all

all : Quast2OO OOChecker ONT_contigmapper ONT_OrientationCorrecterV1 ONT_OrientationCorrecterV2 ONT_OrientationCorrecterV3 TriContigDetect

OOChecker : OOChecker.cpp
	g++ -std=c++11 -g OOChecker.cpp -o OOChecker

ONT_contigmapper : ONT_contigmapper.cpp
	g++ -std=c++11 -g ONT_contigmapper.cpp -o ONT_contigmapper

ONT_OrientationCorrecter : ONT_OrientationCorrecter.cpp ScaffInfo.cpp ScaffInfo.h
	g++ -std=c++11 -g -c ScaffInfo.cpp -o ScaffInfo.o
	g++ -std=c++11 -g ONT_OrientationCorrecter.cpp ScaffInfo.o -o ONT_OrientationCorrecter

ONT_OrientationCorrecterV1 : ONT_OrientationCorrecterV1.cpp ScaffInfo.cpp ScaffInfo.h
	g++ -std=c++11 -g  -c ScaffInfo.cpp -o ScaffInfo.o
	g++ -std=c++11 -g ONT_OrientationCorrecterV1.cpp ScaffInfo.o -o ONT_OrientationCorrecterV1

ONT_OrientationCorrecterV2 : ONT_OrientationCorrecterV2.cpp ScaffInfo.cpp ScaffInfo.h
	g++ -std=c++11 -g -c ScaffInfo.cpp -o ScaffInfo.o
	g++ -std=c++11 -g ONT_OrientationCorrecterV2.cpp ScaffInfo.o -o ONT_OrientationCorrecterV2
ONT_OrientationCorrecterV3 : ONT_OrientationCorrecterV3.cpp ScaffInfo.cpp ScaffInfo.h
	g++ -std=c++11 -g -c ScaffInfo.cpp -o ScaffInfo.o
	g++ -std=c++11 -g ONT_OrientationCorrecterV3.cpp ScaffInfo.o -o ONT_OrientationCorrecterV3

Quast2OO: Quast2OO.cpp contigPairInfo.h contigPairInfo.cpp
	g++ -std=c++11 -g -c  contigPairInfo.cpp -o contigPairInfo.o 
	g++ -std=c++11 -g Quast2OO.cpp contigPairInfo.o -o Quast2OO

TriContigDetect : TriContigDetect.cpp
	g++ -std=c++11 -g TriContigDetect.cpp -o TriContigDetect

clean:
	rm -rf *.o
	rm -rf OOChecker ONT_contigmapper ONT_OrientationCorrecter ONT_OrientationCorrecterV1 ONT_OrientationCorrecterV2 ONT_OrientationCorrecterV3
