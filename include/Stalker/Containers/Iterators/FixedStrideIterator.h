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

#include <Stalker/Containers/Iterators/FixedStrideIteratorBase.h>

template <typename T>
class FixedStrideIterator : public FixedStrideIteratorBase<FixedStrideIterator<T>, T> {
    using Base = FixedStrideIteratorBase<FixedStrideIterator<T>, T>;

public:
    FixedStrideIterator(T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride) {}
    
protected:
    friend Base;
    inline constexpr std::ptrdiff_t _stride() const {
        return Base::_stride;
    }
};