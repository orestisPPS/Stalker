//
// Created by hal9000 on 7/5/24.
//

#ifndef STALKER_CPU_MANAGER_H
#define STALKER_CPU_MANAGER_H
#include "../MachineTopology/CPUTopologyLinux.h"
#include <unordered_map>
#include <mutex>

class CPU_Manager {
public:
    CPU_Manager();
    std::vector<Core *> getCores(unsigned numCores);
    Core* getCore(bool hyperThread = false);
    std::vector<const Core*> getHyperThreadCores(unsigned numCores);
    std::vector<const Core*> getEcoCores(unsigned numCores);
    std::vector<const Thread*> getThreads(unsigned numThreads);
    Thread* getThread();
    void release(std::vector<Core*> cores);
    void release(std::vector<Thread*> threads);
    
    
private:
    CPUTopologyLinux _cpu;
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _hyperThreadCorePool;
    std::unordered_map<Core*, bool> _ecoCorePool;
    std::unordered_map<Thread*, bool> _threadPool;
    std::vector<Core*> _cores;
    std::vector<Thread*> _threads;
    std::mutex _mutex;
    
    
};


#endif //STALKER_CPU_MANAGER_H
