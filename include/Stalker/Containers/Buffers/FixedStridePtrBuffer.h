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

#include "PtrBufferBase.h"
#include "../Iterators/FixedStrideIteratorBase.h"

namespace Stalker::Containers::Buffers {

    template <typename T>
    class FixedStridePtrBuffer : public PtrBufferBase<T, FixedStridePtrBuffer<T>> {
        using Base = PtrBufferBase<T, FixedStridePtrBuffer<T>>;
    public:

        FixedStridePtrBuffer(T* dataStart, std::size_t size, std::ptrdiff_t stride = 1) : Base(dataStart, size, stride) {}

        FixedStridePtrBuffer(const T* dataStart, std::size_t size, std::ptrdiff_t stride = 1) : Base(const_cast<T*>(dataStart), size, stride) {}
    protected:
        friend Base;

        inline T& _at(std::size_t i) {
            return *(this->_dataPtr + i * this->_stride);
        }
        inline const T& _at(std::size_t i) const {
            return *(this->_dataPtr + i * this->_stride);
        }
        inline FixedStrideIterator<T> _begin() {
            return FixedStrideIterator<T>(this->_dataPtr, this->_stride);
        }
        inline FixedStrideIterator<T> _end() {
            return FixedStrideIterator<T>(this->_dataPtr + this->_size * this->_stride, this->_stride);
        }
        inline FixedStrideIterator<const T> _cbegin() const {
            return FixedStrideIterator<const T>(this->_dataPtr, this->_stride);
        }
        inline FixedStrideIterator<const T> _cend() const {
            return FixedStrideIterator<const T>(this->_dataPtr + this->_size * this->_stride, this->_stride);
        }
    };

} // namespace Stalker::Containers::Buffers
