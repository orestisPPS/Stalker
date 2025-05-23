//
// Created by hal9000 on 4/28/24.
//

#ifndef STALKER_TIMER_H
#define STALKER_TIMER_H
#include <chrono>
#include "../Utility/Units/STLKR_Units.h"


class Timer {
public:
    Timer(STLKR_TimeUnit unit) : _unit(unit) {}

    void start() {
        _start = std::chrono::high_resolution_clock::now();
        _running = true;
    }

    void stop() {
        _end = std::chrono::high_resolution_clock::now();
        _running = false;
    }

    STLKR_TimeUnit getUnit() {
        return _unit;
    }

    void setUnit(STLKR_TimeUnit unit) {
        _unit = unit;
    }

    std::chrono::duration<double> duration() {
        if (_running) {
            stop();
        }
        switch (_unit) {
            case STLKR_TimeUnit::nanoseconds:
                return std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _start);
            case STLKR_TimeUnit::microseconds:
                return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start);
            case STLKR_TimeUnit::milliseconds:
                return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
            case STLKR_TimeUnit::seconds:
                return std::chrono::duration_cast<std::chrono::seconds>(_end - _start);
            case STLKR_TimeUnit::minutes:
                return std::chrono::duration_cast<std::chrono::minutes>(_end - _start);
            case STLKR_TimeUnit::hours:
                return std::chrono::duration_cast<std::chrono::hours>(_end - _start);
            case STLKR_TimeUnit::days:
                return std::chrono::duration_cast<std::chrono::hours>(_end - _start) / 24;
            case STLKR_TimeUnit::weeks:
                return std::chrono::duration_cast<std::chrono::hours>(_end - _start) / 168;
        }
        return std::chrono::duration<double>(0);
    }
    
private:
    bool _running = false;
    STLKR_TimeUnit _unit = STLKR_TimeUnit::seconds;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    std::chrono::time_point<std::chrono::high_resolution_clock> _end;
};


#endif //STALKER_TIMER_H
