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

#ifndef INTEGRAL_H
#define INTEGRAL_H

#include "IntegrationMethods.h"

namespace Stalker::Mathematics {

template <IntegrationType Type, typename T>
class Integral {
public:

    constexpr double operator()(const T *f_x, size_t size, double h) const {
        return evaluate(f_x, size, h);
    }

    template <size_t Size>
    constexpr double operator()(const T *f_x, double h) const {
        return evaluate<Size>(f_x, h);
    }

    static constexpr double evaluate(const T *f_x, size_t size, double h) {
        return IntegrationMethod<Type, T>::integrate(f_x, size, h);
    }

    template <size_t Size>
    static constexpr double evaluate(const T *f_x, double h) {
        return IntegrationMethod<Type, T>::template integrate<Size>(f_x, h);
    }
};

} // namespace Stalker::Mathematics
#endif // INTEGRAL_H