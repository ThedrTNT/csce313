#include <pthread.h>
#include "mutex_guard.hpp"

MutexGuard::MutexGuard(Mutex & m) {
	this->m = &m;
	this->m->Lock();
}

MutexGuard::~MutexGuard() {
	m->Unlock();
}