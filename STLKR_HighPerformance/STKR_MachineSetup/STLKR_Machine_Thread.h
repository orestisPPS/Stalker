//
// Created by hal9000 on 6/16/24.
//

#ifndef STALKER_STLKR_MACHINE_THREAD_H
#define STALKER_STLKR_MACHINE_THREAD_H

#include "STLKR_Machine_SharedCache.h"


class STLKR_Machine_Thread {
public:
    STLKR_Machine_Thread() = default;
    STLKR_Machine_Thread(unsigned id, unsigned clockMin, unsigned clockMax, STLKR_Machine_SharedCache *sharedCacheMemory);
    
    unsigned getId() const;
    unsigned getClockMin() const;
    unsigned getClockMax() const;
    bool isRunning() const;
    const STLKR_Machine_SharedCache *getSharedCacheMemory();
private:
    unsigned _id;
    unsigned _clockMin;
    unsigned _clockMax;
    bool _isRunning;
    STLKR_Machine_SharedCache *_sharedCacheMemory;
};


#endif //STALKER_STLKR_MACHINE_THREAD_H
