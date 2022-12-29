.PHONY: all 

CC ?='gcc'
CXX ?='g++'
PREFIX ?='/usr/local/TGS-GapCloser'

all: minimap2 minimap2/libminimap2.a
	cd main_src_ont && make CC=${CC} CXX=${CXX}

minimap2:
	echo "error: please download/link minimap2 into this folder "
	exit 1

minimap2/minimap2:
	cd minimap2 && make

minimap2/libminimap2.a:
	cd minimap2 && make

condainstall: all
	echo  "Installed into ${PREFIX}"
	mkdir -p  ${PREFIX}/bin
	cp tgsgapcloser ${PREFIX}/bin
	chmod a+x ${PREFIX}/bin/tgsgapcloser
	cp -r tgsgapcloserbin ${PREFIX}/bin
	chmod a+x ${PREFIX}/bin/tgsgapcloserbin/*

clean:
	cd main_src_ont && make clean
	rm -rf bin
