//
// Created by hal9000 on 4/28/24.
//

#ifndef STALKER_TIMER_H
#define STALKER_TIMER_H
#include <chrono>
#include "../Units/STLKR_Units.h"


class Timer {
public:
    explicit Timer(STLKR_TimeUnit unit = STLKR_TimeUnit::seconds);
    void start();
    void stop();
    std::chrono::duration<double> duration();
    STLKR_TimeUnit getUnit();
    void setUnit(STLKR_TimeUnit unit);
    
private:
    bool _running = false;
    STLKR_TimeUnit _unit = STLKR_TimeUnit::seconds;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    std::chrono::time_point<std::chrono::high_resolution_clock> _end;
};


#endif //STALKER_TIMER_H
