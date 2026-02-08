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

#include <Stalker/Containers/Iterators/StrideIteratorBase.h>

template <typename Derived, typename T>
class FixedStrideIteratorBase : public StrideIteratorBase<Derived, T> {
    using Base = StrideIteratorBase<Derived, T>;

protected:
    friend Base;

    FixedStrideIteratorBase(T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride) {}

    inline constexpr void _increment() { this->_current += child()._stride(); }
    inline constexpr void _decrement() { this->_current -= child()._stride(); }
    inline constexpr void _advance(typename Base::difference_type n) { this->_current += n * child()._stride(); }
    inline constexpr typename Base::difference_type _distance(const FixedStrideIteratorBase& other) const {
        return (this->_current - other._current) / child()._stride();
    }
    inline constexpr bool _equals(const FixedStrideIteratorBase& other) const { return this->_current == other._current; }
    inline constexpr bool _less_than(const FixedStrideIteratorBase& other) const { return this->_current < other._current; }

    
private:
    inline Derived& child() { return *static_cast<Derived*>(this); }
    inline const Derived& child() const { return *static_cast<const Derived*>(this); }
};
