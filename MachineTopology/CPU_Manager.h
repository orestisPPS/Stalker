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
    explicit CPU_Manager(unsigned availableCores, bool enableHyperthreading = false);
    
    ~CPU_Manager();
    std::vector<Core *> getCores();
    void release(const std::vector<Core*>& cores);
    void enableHyperthreading(bool isEnabled);
    bool isHyperthreadingEnabled() const;
    void setAvailableCores(unsigned availableCores);
    unsigned getAvailableCores() const;
    void printInConsole();
    
private:
    bool _hyperthreadingEnabled;
    unsigned _availableCores;
    CPUTopologyLinux *_cpu;
    std::unordered_map<Core*, bool> _corePool;
    std::unordered_map<Core*, bool> _hyperThreadCorePool;
    std::unordered_map<Core*, bool> _ecoCorePool;
    std::unordered_map<Thread*, bool> _threadPool;
    std::vector<Core*> _cores;
    std::vector<Thread*> _threads;
    
    void _populate();
};


#endif //STALKER_CPU_MANAGER_H
