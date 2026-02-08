/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// \file Stopwatch.h
/// \brief Collection of Timer measurements with seconds-based and unit-valued accessors.
/// \date 2025-09-25
#pragma once

#include <string>
#include <vector>
#include <Stalker/Utility/Timer.h>

namespace Stalker::Utility {

/**
 * \class Stopwatch
 * \brief Aggregates multiple Timer measurements and provides access to their durations.
 *
 * ### Semantics
 * - Stores a vector of \ref Timer snapshots (copied in via \ref registerTimer).
 * - Duration-returning methods yield seconds (`std::chrono::duration<double>`).
 * - Value-returning methods yield raw scalar `double` in a requested \ref Stalker::Core::TimeUnit.
 */
class Stopwatch {
    using TimeUnit = Stalker::Core::TimeUnit;
    using Duration = std::chrono::duration<double>; ///< Seconds-based duration.

public:
    /**
     * \brief Constructs an empty stopwatch with an optional display name.
     * \param name Human-readable identifier.
     * \post `size() == 0`.
     * \complexity Constant.
     */
    explicit Stopwatch(const std::string& name = "") : _name(name) {}

    /// \return Human-readable identifier.
    const std::string& getName() const { return _name; }

    /**
     * \brief Registers (copies) a completed or running \ref Timer measurement.
     * \param timer Timer instance to copy into the internal store.
     * \post `size()` increases by 1.
     * \complexity Amortized constant.
     */
    void registerTimer(const Timer& timer) { _measurements.push_back(timer); }

    /**
     * \brief Access a stored Timer by index (mutable).
     * \param index Zero-based index.
     * \return Reference to stored Timer.
     * \throw std::out_of_range if index ≥ size().
     * \complexity Constant.
     */
    Timer& getTimer(size_t index = 0) { return _measurements.at(index); }

    /**
     * \brief Remove all stored measurements.
     * \post `size() == 0`.
     * \complexity Linear in number of measurements (vector clear).
     */
    void reset() { _measurements.clear(); }

    /**
     * \brief Last measurement as seconds-based duration.
     * \pre `size() > 0`.
     * \param unit Ignored (API symmetry with value overloads).
     * \return `Duration` in seconds.
     * \complexity Constant.
     */
    Duration getLastMeasurement() const {
        return _measurements.back().duration();
    }

    /**
     * \brief Last measurement as scalar in requested unit.
     * \pre `size() > 0`.
     * \param unit Target unit (ns, µs, ms, s, min, h, d, w).
     * \return Elapsed time as `double` in \p unit.
     * \complexity Constant.
     */
    double getLastMeasurementValue(TimeUnit unit = TimeUnit::nanoseconds) const {
        return _measurements.back().durationValue(unit);
    }

    /**
     * \brief Measurement at index as seconds-based duration.
     * \param index Zero-based index.
     *  \return `Duration` in seconds.
     * \throw std::out_of_range if index ≥ size().
     * \complexity Constant.
     */
    Duration getMeasurement(size_t index) const {
        return _measurements.at(index).duration();
    }

    /**
     * \brief Measurement at index as scalar in requested unit.
     * \param index Zero-based index.
     * \param unit Target unit (ns, µs, ms, s, min, h, d, w).
     * \return Elapsed time as `double` in \p unit.
     * \throw std::out_of_range if index ≥ size().
     * \complexity Constant.
     */
    double
    getMeasurementValue(size_t index, TimeUnit unit = TimeUnit::nanoseconds) const {
        return _measurements.at(index).durationValue(unit);
    }

    /**
     * \brief All measurements as seconds-based durations.
     * \return Vector of `Duration` values in seconds.
     * \complexity Linear in `size()`.
     */
    std::vector<Duration> getAllMeasurements() const {
        std::vector<Duration> out;
        out.reserve(_measurements.size());
        for (const auto& t : _measurements) out.push_back(t.duration());
        return out;
    }

    /**
     * \brief All measurements as scalars in requested unit.
     * \param unit Target unit (ns, µs, ms, s, min, h, d, w).
     * \return Vector of `double` values in \p unit.
     * \complexity Linear in `size()`.
     */
    std::vector<double> getAllMeasurementsValues(TimeUnit unit = TimeUnit::seconds) const {
        std::vector<double> out;
        out.reserve(_measurements.size());
        for (const auto& t : _measurements) out.push_back(t.durationValue(unit));
        return out;
    }

    std::vector<std::string>& getTags() { return _tags; }

    const std::vector<std::string>& getTags() const { return _tags; }

private:
    std::string _name; ///< Optional identifier.
    TimeUnit    _unit; ///< Default unit for value accessors.
    std::vector<Timer> _measurements; ///< Stored timer snapshots.
    std::vector<std::string> _tags ; ///< Optional tags associated with this stopwatch.
};

} // namespace Stalker::Utility
