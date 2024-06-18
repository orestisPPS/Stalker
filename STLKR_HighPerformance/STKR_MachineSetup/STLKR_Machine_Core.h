//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_STLKR_MACHINE_CORE_H
#define STALKER_STLKR_MACHINE_CORE_H

#include <vector>
#include "STLKR_Machine_Thread.h"

class STLKR_Machine_Core {
public:
    STLKR_Machine_Core(unsigned id, std::vector<STLKR_Machine_Thread*> threads);
    ~STLKR_Machine_Core() = default;
    unsigned getId() const;
    const std::vector<STLKR_Machine_Thread *> & getThreads() const;
    void setThreadAffinity();
    
    
private:
    unsigned _id;
    std::vector<STLKR_Machine_Thread*> _threads;
    std::vector<pthread_t> _pThreads;
    
    static void* thread_function(void* arg);
};


#endif //STALKER_STLKR_MACHINE_CORE_H

