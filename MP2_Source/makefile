# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
#C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = g++ -std=c++11

#all: dataserver simpleclient client sync_test
all: sync_test

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(C++) -o dataserver dataserver.cpp reqchannel.o -lpthread

simpleclient: simpleclient.cpp reqchannel.o
	$(C++) -o simpleclient simpleclient.cpp reqchannel.o

client: client.cpp reqchannel.o
	$(C++) -o client client.cpp reqchannel.o

sync_test: mutex.cpp mutex.hpp mutex_guard.cpp mutex_guard.hpp sync_test.cpp
	$(C++) -o sync_test mutex.cpp mutex.hpp mutex_guard.cpp mutex_guard.hpp sync_test.cpp -lpthread