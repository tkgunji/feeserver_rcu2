OBJECTS=ctr.o lockguard.o statemachine.o dcscMsgBufferInterface.o dev.o selectmapInterface.o messagebuffer.o issuehandler.o ser.o dim_services.o
OBJECTS_RCU=$(OBJECTS) rcu.o fec.o dcb.o
OBJECTS_RCU2=$(OBJECTS) rcu2.o fec.o
OBJECTS_TPC=$(OBJECTS_RCU) tpc.o tpc_common.o
OBJECTS_TPC2=$(OBJECTS_RCU2) tpc2.o tpc_common.o
OBJECTS_PHS=$(OBJECTS_RCU) phos.o phos_common.o
OBJECTS_PHS2=$(OBJECTS_RCU2) phos2.o phos_common.o
OBJECTS_BB=$(OBJECTS) busybox.o dcb.o
OBJECTS_CPULSER=$(OBJECTS) calibrationpulser.o dcb.o
OBJECTS_GPULSER=$(OBJECTS) gatepulser.o dcb.o
OBJECTS_LASER=$(OBJECTS) laser.o dcb.o
OBJECTS_TOR=$(OBJECTS) triggeror.o dcb.o
SOURCES=ctr.cpp laser.cpp phos.cpp busybox.cpp gatepulser.cpp lockguard.cpp statemachine.cpp calibrationpulser.cpp rcu.cpp tpc.cpp dcscMsgBufferInterface.cpp dev.cpp fec.cpp laser.cpp selectmapInterface.cpp messagebuffer.cpp triggeror.cpp issuehandler.cpp ser.cpp dim_services.cpp tpc_common.cpp phos_common.cpp dcb.cpp
#EXECUTABLE=libCommandCoderTpc.so libCommandCoderTpc2.so libCommandCoderPhs.so libCommandCoderPhs2.so libCommandCoderBb.so libCommandCoderCpulser.so libCommandCoderGpulser.so libCommandCoderLaser.so libCommandCoderTor.so

EXECUTABLE=libCommandCoderTpc2.a libCommandCoderCpulser.a libCommandCoderGpulser.a libCommandCoderLaser.a 
CXX=g++
EXTRAFLAGS=
CFLAGS=-c -fPIC -I../dim $(EXTRAFLAGS)
LDFLAGS=-fPIC -shared -L../dim -ldim -lpthread
LD=ar cru
RANLIB=ranlib
LIBDIM=../dim/libdim.a

ifeq ($(ARCH),arm)
EXECUTABLE=libCommandCoderTpc2.a libCommandCoderCpulser.a libCommandCoderGpulser.a libCommandCoderLaser.a
CXX=arm-uclinuxeabi-g++
EXTRAFLAGS=
#CFLAGS=-c -I../dim $(EXTRAFLAGS) -I${INSTALL_ROOT}/linux/include -g -O2 -mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread
CFLAGS=-c -I../dim $(EXTRAFLAGS) -g -O2 -mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread
LDFLAGS=-mcpu=cortex-m3 -mthumb -L${INSTALL_ROOT}/A2F/root/usr/lib -pthread ../libdim.a
LD=arm-uclinuxeabi-ar cru
RANLIB=arm-uclinuxeabi-ranlib
LIBDIM=../dim/libdim.a
endif 

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

libCommandCoderTpc.so: $(OBJECTS_TPC)
	$(CXX) $(LDFLAGS) $(OBJECTS_TPC) -o $@

libCommandCoderTpc2.so: $(OBJECTS_TPC2)
	$(CXX) $(LDFLAGS) $(OBJECTS_TPC2) -o $@

libCommandCoderTpc2.a: $(OBJECTS_TPC2)
	$(LD) libCommandCoderTpc2.a $(OBJECTS_TPC2)

libCommandCoderPhs.so: $(OBJECTS_PHS)
	$(CXX) $(LDFLAGS) $(OBJECTS_PHS) -o $@

libCommandCoderPhs2.so: $(OBJECTS_PHS2)
	$(CXX) $(LDFLAGS) $(OBJECTS_PHS2) -o $@

libCommandCoderBb.so: $(OBJECTS_BB)
	$(CXX) $(LDFLAGS) $(OBJECTS_BB) -o $@

libCommandCoderCpulser.so: $(OBJECTS_CPULSER)
	$(CXX) $(LDFLAGS) $(OBJECTS_CPULSER) -o $@

libCommandCoderCpulser.a: $(OBJECTS_CPULSER)
	$(LD) libCommandCoderCpulser.a $(OBJECTS_CPULSER)

libCommandCoderGpulser.so: $(OBJECTS_GPULSER)
	$(CXX) $(LDFLAGS) $(OBJECTS_GPULSER) -o $@

libCommandCoderGpulser.a: $(OBJECTS_GPULSER)
	$(LD) libCommandCoderGpulser.a $(OBJECTS_GPULSER)

libCommandCoderLaser.so: $(OBJECTS_LASER)
	$(CXX) $(LDFLAGS) $(OBJECTS_LASER) -o $@
libCommandCoderLaser.a: $(OBJECTS_LASER)
	$(LD) libCommandCoderLaser.a $(OBJECTS_LASER)

libCommandCoderTor.so: $(OBJECTS_TOR)
	$(CXX) $(LDFLAGS) $(OBJECTS_TOR) -o $@

