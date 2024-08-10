//
// Created by hal9000 on 7/5/24.
//

#ifndef STALKER_CPU_MANAGER_H
#define STALKER_CPU_MANAGER_H
#include "CPUTopologyLinux.h"
#include <unordered_map>
#include <mutex>

class CPU_Manager {
public:
    CPU_Manager();
    
    ~CPU_Manager();
    std::vector<Core *> getCores(unsigned numCores, bool areHyperThreads);
    void release(const std::vector<Core*>& cores);
    void enableHyperthreading(bool isEnabled);
    bool hyperThreadingEnabled() const;
    void printInConsole();
    
    
private:
    CPUTopologyLinux *_cpu;
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _hyperThreadCorePool;
    std::unordered_map<Core*, bool> _ecoCorePool;
    std::unordered_map<Thread*, bool> _threadPool;
    std::vector<Core*> _cores;
    std::vector<Thread*> _threads;
    bool _hyperthreadingEnabled;
    
};


#endif //STALKER_CPU_MANAGER_H
