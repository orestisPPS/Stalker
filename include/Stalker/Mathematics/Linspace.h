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

#ifndef LINSPACE_H
#define LINSPACE_H

#include <vector>
#include <stdexcept>
#include <utility> // for std::pair
#include <array>

namespace Stalker::Mathematics {

template <typename T = double, typename StartType, typename EndType>
static inline void linspace(T* result, StartType start, EndType stop, int num = 50, bool endpoint = true) {
    if (num < 0)
        throw std::invalid_argument("linspace: number of samples must be non-negative");

    const T startValue = static_cast<T>(start);
    const T endValue  = static_cast<T>(stop);

    if (num == 0)
        return;

    if (num == 1) {
        result[0] = startValue;
        return;
    }

    const int denom = endpoint ? (num - 1) : num;
    if (denom <= 0)
        throw std::invalid_argument("linspace: invalid number of samples");

    const T step = (endValue - startValue) / denom;
    for (int i = 0; i < num; ++i)
        result[i] = startValue + i * step;
}

template <typename T = double, typename StartType, typename EndType>
static inline void linspace(std::vector<double>& result, StartType start, EndType stop, int num = 50, bool endpoint = true) {
    if (num < 0)
        throw std::invalid_argument("linspace: number of samples must be non-negative");

    result.resize(num);
    linspace(result.data(), start, stop, num, endpoint);
}

template <typename T = double, typename StartType, typename EndType>
static inline std::vector<double> linspace(StartType start, EndType stop, int num = 50, bool endpoint = true) {
    if (num < 0)
        throw std::invalid_argument("linspace: invalid return size");

    std::vector<double> result(num);
    linspace(result.data(), start, stop, num, endpoint);
    return result;
}

namespace Detail {
    template <typename StartType, typename EndType, size_t... I>
    constexpr std::array<double, sizeof...(I)> linspace_impl(StartType s, EndType e, bool ep, std::index_sequence<I...>) {
        constexpr size_t N = sizeof...(I);
        std::array<double, N> result{};
        if constexpr (N == 1) {
            result[0] = s;
        } else {
            const double denom = ep ? (N - 1) : N;
            const double step  = (e - s) / denom;
            ((result[I] = s + I * step), ...);
        }
        return result;
    }
}

template <size_t Num = 50, typename StartType, typename EndType>
constexpr std::array<double, Num> linspace(StartType start, EndType end, bool endpoint = true) {
    static_assert(Num > 0, "N must be greater than 0");
    return Detail::linspace_impl(start, end, endpoint, std::make_index_sequence<Num>{});
}

} // namespace Stalker::Mathematics
#endif // LINSPACE_H