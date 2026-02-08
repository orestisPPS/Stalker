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

template <typename T, bool Ascending = true>
class IndexDependentStrideIterator
  : public DynamicStrideIteratorBase<IndexDependentStrideIterator<T, Ascending>, T> {
    
    using Base = DynamicStrideIteratorBase<IndexDependentStrideIterator<T, Ascending>, T>;

public:
    // Constructor sets the "base offset" plus we default _stride=1 in the base
    IndexDependentStrideIterator(T* startPtr, std::ptrdiff_t baseOffset, std::size_t initialIndex = 0)
        : Base(startPtr, 1), _baseOffset(baseOffset) {
        this->_index = initialIndex;
    }

protected:
    friend Base;
    std::ptrdiff_t _baseOffset;

    inline std::ptrdiff_t _strideFunction(std::size_t currentIndex) const {
        if constexpr (Ascending)
            return _baseOffset + static_cast<std::ptrdiff_t>(currentIndex);
        else
            return _baseOffset - static_cast<std::ptrdiff_t>(currentIndex);
    }

    inline void _advanceFunction(typename Base::difference_type n) {
        for (std::ptrdiff_t i = 0; i < n; ++i)
            this->_increment();
    }
};

} // namespace Stalker::Containers::Iterators