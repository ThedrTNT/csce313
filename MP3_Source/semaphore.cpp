// Ryan Parker

#include "semaphore.hpp"
#include <pthread.h>

Semaphore::Semaphore(int _val) {
  value = _val;
  c = PTHREAD_COND_INITIALIZER;
  m = PTHREAD_MUTEX_INITIALIZER;
}

Semaphore::~Semaphore() {
  pthread_mutex_destroy(&m);
}

int Semaphore::P() {
  pthread_mutex_lock(&m);
  value--;
  if(value < 0) {
    pthread_cond_wait(&c, &m);
  }
  pthread_mutex_unlock(&m);
  return 0;
}

int Semaphore::V() {
  pthread_mutex_lock(&m);
  value++;
  if(value <= 0) {
    pthread_cond_signal(&c);
  }
  pthread_mutex_unlock(&m);
  return 0;
}