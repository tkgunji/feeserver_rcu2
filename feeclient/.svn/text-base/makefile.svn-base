OBJECTS_FEECLIENT=AckData.o FeeAckInfo.o FeeClientCore.o FeeClientLibInterface.o FeeClientLogger.o FeeInfo.o FeeItemInfo.o FeeMsgInfo.o FeePacket.o FeeSampleClient.o LockGuard.o
SOURCES_FEECLIENT=AckData.cpp FeeAckInfo.cpp FeeClientCore.cpp FeeClientLibInterface.cpp FeeClientLogger.cpp FeeInfo.cpp FeeItemInfo.cpp FeeMsgInfo.cpp FeePacket.cpp FeeSampleClient.cpp LockGuard.cpp
EXECUTABLE_FEECLIENT=libfeeclient.so
OBJECTS_FEESERVERCTRL=feeserver-ctrl.o
SOURCES_FEESERVERCTRL=feeserver-ctrl.cpp
EXECUTABLE_FEESERVERCTRL=feeserver-ctrl
CXX=g++
EXTRAFLAGS=
CFLAGS=-c -fPIC -I../dim $(EXTRAFLAGS)
LDFLAGS_FEECLIENT=-fPIC -shared -L../dim -ldim -lpthread
LDFLAGS_FEESERVERCTRL=-fPIC -L. -L../dim -ldim -lfeeclient -lpthread

all: $(EXECUTABLE_FEECLIENT) $(EXECUTABLE_FEESERVERCTRL) 

clean:
	rm -f *.o
	rm -f *.so
	rm -f feeserver-ctrl

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

$(EXECUTABLE_FEECLIENT): $(OBJECTS_FEECLIENT)
	$(CXX) $(LDFLAGS_FEECLIENT) $(OBJECTS_FEECLIENT) -o $@

$(EXECUTABLE_FEESERVERCTRL): $(OBJECTS_FEESERVERCTRL)
	$(CXX) $(LDFLAGS_FEESERVERCTRL) $(OBJECTS_FEESERVERCTRL) -o $@

