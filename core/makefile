OBJECTS=feeserver.o main.o
SOURCES=feeserver.cc main.cc
EXECUTABLE=feeserver_rcu2 feeserver_laser feeserver_cpulser feeserver_gpulser 
#CXX=g++
CXX=arm-uclinuxeabi-g++
EXTRAFLAGS=
CFLAGS=-c -I../dim $(EXTRAFLAGS) -I$(INSTALL_ROOT)/A2F/root/usr/include -g -O2 -mcpu=cortex-m3 -mthumb -L/rcu2/release/linux-cortexm-1.11.0/A2F/root/usr/lib -pthread
LDFLAGS=-mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread
LIBDIM=../dim/libdim.a 
LIBCC_RCU2=../controlengine/libCommandCoderTpc2.a
LIBCC_Laser=../controlengine/libCommandCoderLaser.a
LIBCC_Cpulser=../controlengine/libCommandCoderCpulser.a
LIBCC_Gpulser=../controlengine/libCommandCoderGpulser.a






all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)

.cc.o:
	$(CXX) $(CFLAGS) $< -o $@

feeserver_rcu2: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBDIM) $(LIBCC_RCU2) -o $@
feeserver_laser: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBDIM) $(LIBCC_Laser) -o $@
feeserver_cpulser: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBDIM) $(LIBCC_Cpulser) -o $@
feeserver_gpulser: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBDIM) $(LIBCC_Gpulser) -o $@

