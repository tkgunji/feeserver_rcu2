ARCH=x86
CXX=g++
CC=gcc
EXTRAFLAGS="-ggdb3 -Wall "

MIPSE=MIPSEL

all:
	mkdir -p bin/$(ARCH)
	make -C dim            CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS) MIPSE=$(MIPSE)
	cp dim/libdim.* dim/dns bin/$(ARCH)
	make -C controlengine  CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp controlengine/libCommandCoder*.* bin/$(ARCH)
	make -C core           CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp core/feeserver_* bin/$(ARCH)
	make -C feeclient/dim      CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp feeclient/dim/libdim.so bin/$(ARCH)
	make -C feeclient      CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp feeclient/libfeeclient.so feeclient/feeserver-ctrl bin/$(ARCH)
#	doxygen doxygen

feeserver:
	mkdir -p bin/$(ARCH)
	make -C dim            CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS) MIPSE=$(MIPSE)
	cp dim/libdim.a dim/dns bin/$(ARCH)
	make -C controlengine  CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp controlengine/libCommandCoder*.a bin/$(ARCH)
	make -C core           CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp core/feeserver_* bin/$(ARCH)

feecontrol:
	make -C feeclient/dim      CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp feeclient/dim/libdim.so bin/$(ARCH)
	make -C feeclient      CXX=$(CXX) CC=$(CC) EXTRAFLAGS=$(EXTRAFLAGS)
	cp feeclient/libfeeclient.so feeclient/feeserver-ctrl bin/$(ARCH)

clean:
	make -C dim            clean
	make -C core           clean
	make -C controlengine  clean
	make -C feeclient/dim  clean
	make -C feeclient      clean
	rm -rf bin/x86
	rm -rf bin/arm
	rm -rf html


arm:
#	make ARCH=arm CXX=arm-uclibc-g++ CC=arm-uclibc-gcc EXTRAFLAGS=$(EXTRAFLAGS) MIPSE=$(MIPSE)
	make feeserver ARCH=arm CXX=arm-uclinuxeabi-g++ CC=arm-uclinuxeabi-gcc EXTRAFLAGS=$(EXTRAFLAGS) MIPSE=$(MIPSE)
	make feecontrol ARCH=arm CXX=g++ CC=gcc EXTRAFLAGS=$(EXTRAFLAGS)
