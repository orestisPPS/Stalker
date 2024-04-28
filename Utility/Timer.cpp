//
// Created by hal9000 on 4/28/24.
//

#include "Timer.h"

Timer::Timer(STLKR_TimeUnit unit) : _unit(unit) {}

void Timer::start() {
    _start = std::chrono::high_resolution_clock::now();
    _running = true;
}

void Timer::stop() {
    _end = std::chrono::high_resolution_clock::now();
    _running = false;
}

STLKR_TimeUnit Timer::getUnit() {
    return _unit;
}

void Timer::setUnit(STLKR_TimeUnit unit) {
    _unit = unit;
}

std::chrono::duration<double> Timer::duration() {
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
