//
// Created by hal9000 on 6/16/24.
//

#include "STLKR_Machine_Thread.h"

STLKR_Machine_Thread::  STLKR_Machine_Thread(unsigned id, unsigned clockMin, unsigned clockMax, STLKR_Machine_SharedCache *sharedCacheMemory){
    _id = id;
    _clockMin = clockMin;
    _clockMax = clockMax;
    _sharedCacheMemory = sharedCacheMemory;
    _isRunning = false;
    _pThread = 0;
    _pThreadAttribute = {};
}

unsigned STLKR_Machine_Thread::getClockMin() const{
    return _clockMin;
}

unsigned STLKR_Machine_Thread::getClockMax() const{
    return _clockMax;
}

bool STLKR_Machine_Thread::isRunning() const{
    return _isRunning;
}

void STLKR_Machine_Thread::join() const{
    pthread_join(_pThread, nullptr);
}

const STLKR_Machine_SharedCache *STLKR_Machine_Thread::getSharedCacheMemory(){
    return _sharedCacheMemory;
}

void STLKR_Machine_Thread::addToCoreSet(unsigned coreId, cpu_set_t &coreSet){
    CPU_SET(coreId, &coreSet);
}

void STLKR_Machine_Thread::setThreadAffinity(cpu_set_t &coreSet){
    _initializeAttribute(_pThreadAttribute);
    int result = pthread_attr_setaffinity_np(&_pThreadAttribute, sizeof(cpu_set_t), &coreSet);
    if (result != 0) std::cerr << "Error setting affinity for thread " << _id << std::endl;
}

void STLKR_Machine_Thread::resetThreadAffinity(){
    cpu_set_t zeroSet;
    CPU_ZERO(&zeroSet);
    int result = pthread_attr_setaffinity_np(&_pThreadAttribute, sizeof(cpu_set_t), &zeroSet);
    if (result != 0) std::cerr << "Error resetting affinity for thread " << _id << std::endl;
    _destroyAttribute(_pThreadAttribute);
}

unsigned STLKR_Machine_Thread::getId() const{
    return _id;
}

void STLKR_Machine_Thread::_initializeAttribute(pthread_attr_t &attribute) {
    pthread_attr_init(&attribute);
}

void STLKR_Machine_Thread::_destroyAttribute(pthread_attr_t &attribute) {
    pthread_attr_destroy(&attribute);
}
