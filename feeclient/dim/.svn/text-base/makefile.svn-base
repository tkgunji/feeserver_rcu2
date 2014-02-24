OBJECTS_DIM=conn_handler.o copy_swap.o diccpp.o dic.o dimcpp.o dim_thr.o discpp.o dis.o dll.o dna.o dtq.o hash.o open_dns.o sll.o swap.o tcpip.o tokenstring.o utilities.o
SOURCES_DIM=conn_handler.c copy_swap.c dic.c diccpp.cxx dimcpp.cxx dim_thr.c dis.c discpp.cxx dll.c dna.c dtq.c hash.c open_dns.c sll.c swap.c tcpip.c tokenstring.cxx utilities.c
EXECUTABLE_DIM=libdim.so
OBJECTS_DNS=dns.o
SOURCES_DNS=dns.c
EXECUTABLE_DNS=dns
CC=gcc
CXX=g++
MIPSE=MIPSEL
EXTRAFLAGS=
CFLAGS=-c -fPIC -I. -D$(MIPSE) -DPROTOCOL=1 $(EXTRAFLAGS) 
LDFLAGS_DIM=-shared -fPIC -L.
LDFLAGS_DNS=-fPIC -L. -ldim -lpthread

all: $(EXECUTABLE_DIM) $(EXECUTABLE_DNS)

clean:
	rm -f *.o
	rm -f *.so
	rm -f dns

dns.o: dns.c
	$(CC) $(CFLAGS) -o dns.o dns.c

dns: $(OBJECTS_DNS)
	$(CXX) $(LDFLAGS_DNS) $(OBJECTS_DNS) -o $@

conn_handler.o: conn_handler.c
	$(CC) $(CFLAGS) -o conn_handler.o conn_handler.c

copy_swap.o: copy_swap.c
	$(CC) $(CFLAGS) -o copy_swap.o copy_swap.c

diccpp.o: diccpp.cxx
	$(CXX) $(CFLAGS) -o diccpp.o diccpp.cxx

dic.o: dic.c
	$(CC) $(CFLAGS) -o dic.o dic.c

dimcpp.o: dimcpp.cxx
	$(CXX) $(CFLAGS) -o dimcpp.o dimcpp.cxx

dim_thr.o: dim_thr.c
	$(CC) $(CFLAGS) -o dim_thr.o dim_thr.c

discpp.o: discpp.cxx
	$(CXX) $(CFLAGS) -o discpp.o discpp.cxx

dis.o: dis.c
	$(CC) $(CFLAGS) -o dis.o dis.c

dll.o: dll.c
	$(CC) $(CFLAGS) -o dll.o dll.c

dna.o: dna.c
	$(CC) $(CFLAGS) -o dna.o dna.c

dtq.o: dtq.c
	$(CC) $(CFLAGS) -o dtq.o dtq.c

hash.o: hash.c
	$(CC) $(CFLAGS) -o hash.o hash.c

open_dns.o: open_dns.c
	$(CC) $(CFLAGS) -o open_dns.o open_dns.c

sll.o: sll.c
	$(CC) $(CFLAGS) -o sll.o sll.c

swap.o: swap.c
	$(CC) $(CFLAGS) -o swap.o swap.c

tcpip.o: tcpip.c
	$(CC) $(CFLAGS) -o tcpip.o tcpip.c

tokenstring.o: tokenstring.cxx
	$(CXX) $(CFLAGS) -o tokenstring.o tokenstring.cxx

utilities.o: utilities.c
	$(CC) $(CFLAGS) -o utilities.o utilities.c

libdim.so: $(OBJECTS_DIM)
	$(CXX) $(LDFLAGS_DIM) $(OBJECTS_DIM) -o $@

