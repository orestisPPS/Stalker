//
// Created by hal9000 on 4/28/24.
//
#pragma once

#include <chrono>
#include <Stalker/Core/Units.h>

namespace Stalker::Utility {

class Timer {
public:
    Timer(TimeUnit unit) : _unit(unit) {}

    inline void start() {
        _start = std::chrono::high_resolution_clock::now();
        _running = true;
    }

    inline void stop() {
        _end = std::chrono::high_resolution_clock::now();
        _running = false;
    }

    TimeUnit getUnit() {
        return _unit;
    }

    void setUnit(TimeUnit unit) {
        _unit = unit;
    }

    inline std::chrono::duration<double> duration() {
        if (_running) {
            stop();
        }
        switch (_unit) {
            case TimeUnit::nanoseconds:
                return std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _start);
            case TimeUnit::microseconds:
                return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start);
            case TimeUnit::milliseconds:
                return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
            case TimeUnit::seconds:
                return std::chrono::duration_cast<std::chrono::seconds>(_end - _start);
            case TimeUnit::minutes:
                return std::chrono::duration_cast<std::chrono::minutes>(_end - _start);
            case TimeUnit::hours:
                return std::chrono::duration_cast<std::chrono::hours>(_end - _start);
            case TimeUnit::days:
                return std::chrono::duration_cast<std::chrono::hours>(_end - _start) / 24;
            case TimeUnit::weeks:
                return std::chrono::duration_cast<std::chrono::hours>(_end - _start) / 168;
        }
        return std::chrono::duration<double>(0);
    }
    
private:
    bool _running = false;
    TimeUnit _unit = TimeUnit::seconds;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    std::chrono::time_point<std::chrono::high_resolution_clock> _end;
};

} // namespace Stalker::Utility