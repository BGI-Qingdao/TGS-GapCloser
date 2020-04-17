.PHONY: all

PREFIX ?='/usr/local/TGS-GapCloser'

all: minimap2 minimap2/libminimap2.a minimap2/minimap2
	cd main_src_ont && make
	cp minimap2/minimap2 bin/

minimap2:
	echo "error: please download/link minimap2 into this folder "
	exit 1

minimap2/minimap2:
	cd minimap2 && make

minimap2/libminimap2.a:
	cd minimap2 && make

condainstall: all
	echo  "Installed into ${PREFIX}"
	mkdir -p  ${PREFIX}
	chmod a+x TGS-GapCloser
	cp TGS-GapCloser ${PREFIX}/
	chmod a+x ${PREFIX}/TGS-GapCloser
	cp -r bin ${PREFIX}/tgsgapcloserbin
	chmod a+x ${PREFIX}/tgsgapcloserbin/*

clean:
	cd main_src_ont && make clean
	rm -rf bin
