//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_STLKR_MACHINE_CORE_H
#define STALKER_STLKR_MACHINE_CORE_H

#include <vector>
#include "STLKR_Machine_CacheLevel.h"
#include "STLKR_Machine_Thread.h"

class STLKR_Machine_Core {
public:
    STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread> threads);
private:
    unsigned _id;
    std::vector<STLKR_Machine_Thread> _threads;
};


#endif //STALKER_STLKR_MACHINE_CORE_H

