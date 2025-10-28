/// \file Timer.h
/// \brief Monotonic wall-clock timer for elapsed-time measurement (seconds + unit-valued queries).
/// \date 2025-09-25
#pragma once

#include <chrono>
#include <type_traits>
#include <Stalker/Core/Units.h>

namespace Stalker::Utility {


/**
 * \class Timer
 * \brief Measures elapsed time between start() and stop() with a monotonic clock.
 *
 * - Clock: selects the highest-resolution monotonic clock available at compile time.
 * - duration(unit): returns elapsed time as seconds (`std::chrono::duration<double>`).
 * - durationValue(unit): returns a scalar `double` in the requested unit.
 *
 * Thread-safety: not thread-safe. Exception-safety: `noexcept`.
 */
class Timer {
    /// Best available monotonic clock (prefer high_resolution_clock if steady).
    using Clock = std::conditional_t<
        std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock,
        std::chrono::steady_clock>;

    using TimePoint = Clock::time_point;
    using TimeUnit  = Stalker::Core::TimeUnit;
    using Duration = std::chrono::duration<double>;

public:
    Timer() = default;

    /// Starts the timer.
    inline void start() noexcept {
        _start = Clock::now();
    }

    /// Stops the timer.
    inline void stop() noexcept {
        _end = Clock::now();
    }

    /**
     * \brief Elapsed duration in seconds.
     * \return `std::chrono::duration<double>` in seconds.
     */
    std::chrono::duration<double> duration() const {
        return std::chrono::duration<double>(_end - _start);
    }

    /**
     * \brief Elapsed time as a scalar in the requested unit.
     * \param unit Target unit (ns, µs, ms, s, min, h, d, w).
     * \return `double` value in \p unit.
     */
    double durationValue(TimeUnit unit = TimeUnit::seconds) const {
        if (unit == TimeUnit::seconds && _unit != TimeUnit::seconds) {
            unit = _unit;
        }
        const auto delta = _end - _start;
        switch (unit) {
            case TimeUnit::nanoseconds:
                return std::chrono::duration<double, std::nano>(delta).count();
            case TimeUnit::microseconds:
                return std::chrono::duration<double, std::micro>(delta).count();
            case TimeUnit::milliseconds:
                return std::chrono::duration<double, std::milli>(delta).count();
            case TimeUnit::seconds:
                return std::chrono::duration<double>(delta).count();
            case TimeUnit::minutes:
                return std::chrono::duration<double, std::ratio<60>>(delta).count();
            case TimeUnit::hours:
                return std::chrono::duration<double, std::ratio<3600>>(delta).count();
            case TimeUnit::days:
                return std::chrono::duration<double, std::ratio<86400>>(delta).count();
            case TimeUnit::weeks:
                return std::chrono::duration<double, std::ratio<604800>>(delta).count();
        }
        return 0.0;
    }

private:
    TimePoint _start{};        ///< Start timestamp (set by start()).
    TimePoint _end{};          ///< End timestamp (set by stop()).
    TimeUnit _unit = TimeUnit::seconds; ///< Unit for durationValue() (default: seconds).
};

} // namespace Stalker::Utility
