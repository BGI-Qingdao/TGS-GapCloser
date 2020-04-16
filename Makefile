.PHONY: all 

PREFIX ?='/usr/local/TGS-GapCloser'

all: minimap2 minimap2/libminimap2.a
	cd main_src_ont && make 

minimap2:
	echo "error: please download/link minimap2 into this folder "
	exit 1

minimap2/libminimap2.a:
	cd minimap2 && make

install: all
	echo  "Installed into ${PREFIX}"
	mkdir -p  ${PREFIX}
	chmod u+x TGS-GapCloser.sh
	cp TGS-GapCloser.sh ${PREFIX}/
	cp -r bin ${PREFIX}/

clean:
	cd main_src_ont && make clean
	rm -rf bin
