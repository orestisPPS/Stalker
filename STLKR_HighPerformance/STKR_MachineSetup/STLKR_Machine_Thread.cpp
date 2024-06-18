//
// Created by hal9000 on 6/16/24.
//

#include "STLKR_Machine_Thread.h"

STLKR_Machine_Thread::  STLKR_Machine_Thread(unsigned id, unsigned clockMin, unsigned clockMax, STLKR_Machine_SharedCache *sharedCacheMemory) 
: _id(id), _clockMin(clockMin), _clockMax(clockMax), _isRunning(false), _sharedCacheMemory(sharedCacheMemory) {}

unsigned STLKR_Machine_Thread::getId() const{
    return _id;
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

const STLKR_Machine_SharedCache *STLKR_Machine_Thread::getSharedCacheMemory(){
    return _sharedCacheMemory;
}
