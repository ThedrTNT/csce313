#include <pthread.h>
#include "mutex.hpp"


Mutex::Mutex() {
	m = PTHREAD_MUTEX_INITIALIZER;
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&m);
}

void Mutex::Lock() {
	pthread_mutex_lock(&m);
}

void Mutex::Unlock() {
	pthread_mutex_unlock(&m);
}