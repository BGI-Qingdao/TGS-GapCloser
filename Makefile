.PHONY: all 

all: minimap2 minimap2/libminimap2.a
	cd main_src_ont && make 

minimap2:
	echo "error: please download/link minimap2 into this folder "
	exit 1

minimap2/libminimap2.a:
	cd minimap2 && make

clean:
	cd main_src_ont && make clean
	rm -rf bin
