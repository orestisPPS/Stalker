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
    std::vector<STLKR_Machine_Thread *> getThreads(bool hyperThreadEnabled = true) const;
    unsigned getThreadCount(bool hyperThreadEnabled) const;
    void setThreadAffinity();
    void resetThreadAffinity();
    void inline checkThreadAffinity();
    template <typename threadJob>
    void executeJob(threadJob job, unsigned startIndex, unsigned endIndex, bool hyperThreadEnabled = true);

    template<typename threadJob>
    struct JobArgs {
        threadJob job;
        unsigned startIndex;
        unsigned endIndex;
    };
private:
    unsigned _id;
    std::vector<STLKR_Machine_Thread*> _threads;
    std::vector<pthread_t> _pThreads;
    std::vector<pthread_attr_t> _pThreadAttributes;
    cpu_set_t _thisCoreSet;
    void inline _initializeThreadAttributes();

    template<typename threadJob>
    static void* threadFunctionWrapper(void* args);
};
#endif //STALKER_STLKR_MACHINE_CORE_H

