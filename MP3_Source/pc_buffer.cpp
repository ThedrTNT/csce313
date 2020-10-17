// Ryan Parker

#include "pc_buffer.hpp"
#include "semaphore.hpp"
#include <pthread.h>

PCBuffer::PCBuffer(int _size) {
    size = _size;
    mutex = new Semaphore(1);
    full = new Semaphore(0);
    empty = new Semaphore(size);
  }

PCBuffer::~PCBuffer() {
  delete mutex;
  delete full;
  delete empty;
}

int PCBuffer::Deposit(string _item) {
  empty->P();
  mutex->P();
  buffer.push(_item);
  mutex->V();
  full->V();
}

string PCBuffer::Retrieve() {
	full->P();
	mutex->P();
	string ret = buffer.front();
	buffer.pop();
	mutex->V();
	empty->V();
	return ret;
}

