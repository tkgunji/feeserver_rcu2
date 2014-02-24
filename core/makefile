OBJECTS=feeserver.o main.o
SOURCES=feeserver.cc main.cc
#EXECUTABLE=feeserver
CXX=g++
EXTRAFLAGS=
CFLAGS=-c -fPIC -I../dim $(EXTRAFLAGS)
LDFLAGS=-fPIC -L../dim -ldim -lpthread -ldl -rdynamic
LIBCC_RCU2=../controlengine/libCommandCoderTpc2.a
LIBCC_Laser=../controlengine/libCommandCoderLaser.a
LIBCC_Cpulser=../controlengine/libCommandCoderCpulser.a
LIBCC_Gpulser=../controlengine/libCommandCoderGpulser.a
CXX=g++
EXECUTABLE=feeserver_rcu2 feeserver_laser feeserver_cpulser feeserver_gpulser 
LIBDIM=../dim/libdim.a 

ifeq ($(ARCH),arm)
CXX=arm-uclinuxeabi-g++
EXECUTABLE=feeserver_rcu2 feeserver_laser feeserver_cpulser feeserver_gpulser 
CFLAGS=-c -I../dim $(EXTRAFLAGS) -I$(INSTALL_ROOT)/A2F/root/usr/include -g -O2 -mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread
LDFLAGS=-mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread
LIBDIM=../dim/libdim.a 
LIBCC_RCU2=../controlengine/libCommandCoderTpc2.a
LIBCC_Laser=../controlengine/libCommandCoderLaser.a
LIBCC_Cpulser=../controlengine/libCommandCoderCpulser.a
LIBCC_Gpulser=../controlengine/libCommandCoderGpulser.a
endif


all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE) *.gdb

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
feeserver: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@
