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

#include <Stalker/Containers/Iterators/DynamicStrideIteratorBase.h>


namespace Stalker::Containers::Iterators {
template <typename T, bool Ascending>
class LinearStrideIterator : public DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T> {
    using Base = DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T>;

public:                                                                                                
    LinearStrideIterator(T* startPtr, std::ptrdiff_t startStride, size_t index = 0) : Base(startPtr, 1), _startStride(startStride) {
        this->_index = index;
    }

protected:
    friend Base;
    std::ptrdiff_t _startStride; ///< Initial stride value.

    inline std::ptrdiff_t _strideFunction(size_t index) const {
        if constexpr (Ascending)
            return _startStride + index * this->_stride;
        else
            return _startStride - index * this->_stride; 
    }
};

} // namespace Stalker::Containers::Iterators
