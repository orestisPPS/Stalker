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

#pragma once
#include <cmath>

namespace Stalker::Core {

enum class ErrorType {
    Absolute,
    Relative,
    Percent
};

template<typename T, typename ResultT = double>
constexpr ResultT errorAbsolute(T expected, T actual) {
    return std::abs(expected - actual);
}

template<typename T, typename ResultT = double>
constexpr ResultT errorRelative(T expected, T actual) {
    return errorAbsolute<T, ResultT>(expected, actual) / expected;
}

template<typename T, typename ResultT = double>
constexpr ResultT errorPercent(T expected, T actual) {
    return errorRelative<T, ResultT>(expected, actual) * 100;
}

template<typename T, typename ResultT = double>
constexpr ResultT error(T expected, T actual, ErrorType type = ErrorType::Percent) {
    switch(type) {
        case ErrorType::Absolute: return errorAbsolute<T, ResultT>(expected, actual);
        case ErrorType::Relative: return errorRelative<T, ResultT>(expected, actual);
        case ErrorType::Percent:
        default: return errorPercent<T, ResultT>(expected, actual);
    }
}

} // namespace Stalker::Core
