// Ryan Parker

#include "pcbuffer.hpp"
#include <pthread.h>

PCBuffer::PCBuffer(int _size) {
  
}

PCBuffer::~PCBuffer() {
  
}

void PCBuffer::Deposit(string _item) {
  empty.P();
  mutex.P();
  buffer.push(_item);
  mutex.V();
  full.V();
}

string PCBuffer::Retrieve() {
  full.P();
  mutex.P();
  string ret = buffer.front();
  buffer.pop();
  mutex.V();
  empty.V();
  return ret;
}