OBJECTS=feeserver.o main.o
SOURCES=feeserver.cc main.cc
EXECUTABLE=feeserver
#CXX=g++
CXX=arm-uclinuxeabi-g++
EXTRAFLAGS=
CFLAGS=-c -I../dim $(EXTRAFLAGS) -I$(INSTALL_ROOT)/A2F/root/usr/include -g -O2 -mcpu=cortex-m3 -mthumb -L/rcu2/release/linux-cortexm-1.11.0/A2F/root/usr/lib -pthread
LDFLAGS=-mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread
LIBDIM=../dim/libdim.a 
LIBCC=../controlengine/libCommandCoderTpc2.a


all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)

.cc.o:
	$(CXX) $(CFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBDIM) $(LIBCC) -o $@
