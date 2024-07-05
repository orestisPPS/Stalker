//
// Created by hal9000 on 6/16/24.
//

#include "Thread.h"

Thread::Thread(unsigned id, unsigned parentId, unsigned clockMin, unsigned clockMax, SharedCache *sharedCacheMemory){
    _id = id;
    _parentId = parentId;
    _clockMin = clockMin;
    _clockMax = clockMax;
    _sharedCacheMemory = sharedCacheMemory;
    _isRunning = false;
    _pThread = 0;
    _pThreadAttribute = {};
}

unsigned Thread::getClockMin() const{
    return _clockMin;
}

unsigned Thread::getClockMax() const{
    return _clockMax;
}

bool Thread::isRunning() const{
    return _isRunning;
}

void Thread::join() const{
    pthread_join(_pThread, nullptr);
}

const SharedCache *Thread::getSharedCacheMemory(){
    return _sharedCacheMemory;
}


void Thread::setThreadAffinity(cpu_set_t &coreSet) {
    _initializeAttribute(_pThreadAttribute);
    CPU_SET(_id, &coreSet);
    int result = pthread_attr_setaffinity_np(&_pThreadAttribute, sizeof(cpu_set_t), &coreSet);
    if (result != 0) std::cerr << "Error setting affinity for thread " << _id << std::endl;
    _isRunning = true;
    _coreSet = coreSet;
}

void Thread::resetThreadAffinity(){
    CPU_ZERO(&_coreSet);
    int result = pthread_attr_setaffinity_np(&_pThreadAttribute, sizeof(cpu_set_t), &_coreSet);
    if (result != 0) std::cerr << "Error resetting affinity for thread " << _id << std::endl;
    _destroyAttribute(_pThreadAttribute);
}

cpu_set_t Thread::getCoreSet() const{
    return _coreSet;
}

unsigned Thread::getId() const{
    return _id;
}

void Thread::_initializeAttribute(pthread_attr_t &attribute) {
    pthread_attr_init(&attribute);
}

void Thread::_destroyAttribute(pthread_attr_t &attribute) {
    pthread_attr_destroy(&attribute);
}
