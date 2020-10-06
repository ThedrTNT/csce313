# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
# C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = /usr/bin/g++ -std=c++11

.DEFAULT_GOAL := all

clean:
	rm -f *.o sync_test_semaphore sync_test_pcbuffer

all: dataserver simpleclient client test_suite

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp 

dataserver: dataserver.cpp reqchannel.o 
	$(C++) -o dataserver dataserver.cpp reqchannel.o -lpthread

simpleclient: simpleclient.cpp reqchannel.o
	$(C++) -o simpleclient simpleclient.cpp reqchannel.o

client: client.cpp reqchannel.o
	$(C++) -o client client.cpp reqchannel.cpp

semaphore.o: semaphore.hpp semaphore.cpp
	$(C++) -c -g semaphore.cpp -lpthread

pc_buffer.o: pcbuffer.hpp pcbuffer.cpp semaphore.hpp
	$(C++) -c -g pcbuffer.cpp 

test_suite: sync_test_semaphore #sync_test_pcbuffer

sync_test_semaphore: sync_test.cpp semaphore.o
	$(C++) -DSEMAPHORE_TEST -o sync_test_semaphore sync_test.cpp semaphore.o -lpthread

sync_test_pcbuffer: sync_test.cpp pc_buffer.o semaphore.o
	$(C++) -DPCBUFFER_TEST -o sync_test_pcbuffer sync_test.cpp pc_buffer.o semaphore.o -lpthread

