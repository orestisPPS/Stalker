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
#include <memory>
#include <vector>
#include <limits>
#include <stdexcept>
#include <utility>
#include <Stalker/Core/Config/Alignment.h>

namespace Stalker::Memory {

using namespace Stalker::Core::Config;

namespace Detail {

    [[nodiscard]] constexpr size_t checkedAdd(size_t lhs, size_t rhs) {
        return (rhs > std::numeric_limits<size_t>::max() - lhs)
            ? throw std::overflow_error("Aligned allocation request exceeds size_t range")
            : lhs + rhs;
    }

    [[nodiscard]] constexpr size_t checkedMul(size_t lhs, size_t rhs) {
        return (lhs != 0 && rhs > std::numeric_limits<size_t>::max() / lhs)
            ? throw std::overflow_error("Aligned allocation request exceeds size_t range")
            : lhs * rhs;
    }

    template<typename T, size_t Alignment>
    [[nodiscard]] constexpr size_t computePadding(size_t count) {
        static_assert(Alignment != 0 && (Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two.");

        if (count == 0) {
            return 0;
        }

        constexpr size_t elementSize = sizeof(T);
        const size_t bytesRequested = checkedMul(count, elementSize);
        const size_t remainder = bytesRequested % Alignment;

        if (remainder == 0) {
            return 0;
        }

        const size_t missingBytes = Alignment - remainder;
        const size_t numerator = checkedAdd(missingBytes, elementSize - 1);
        return numerator / elementSize;
    }
}

template<typename T, size_t Alignment = DefaultAlignment()>
[[nodiscard]] constexpr size_t getPadding(size_t count) {
    return Detail::computePadding<T, Alignment>(count);
}


template<typename T, size_t Alignment = alignof(T)>
struct AlignedAllocator
{
    static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two");
    static_assert(Alignment >= alignof(T), "Alignment must be >= alignof(T)");

    using value_type      = T;
    using is_always_equal = std::true_type;

    constexpr AlignedAllocator() noexcept = default;
    template<class U>
    constexpr AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    template<class U>
    struct rebind { using other = AlignedAllocator<U, Alignment>; };

    static T* allocate(size_t n) {
        if (n == 0) return nullptr;
        return static_cast<T*>(
            ::operator new(n * sizeof(T), std::align_val_t{Alignment})
        );                              // throws std::bad_alloc on failure
    }

    static void deallocate(T* p, size_t = 0) noexcept {
        ::operator delete(p, std::align_val_t{Alignment});
    }

    bool operator==(const AlignedAllocator&) const noexcept { return true; }
    bool operator!=(const AlignedAllocator&) const noexcept { return false; }
};


template<typename T, size_t Alignment = DefaultAlignment()>
struct AlignedDeleter {
    void operator()(T* p) const noexcept {
        AlignedAllocator<T, Alignment>::deallocate(p);
    }
};


template<typename T, size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline T* createAlignedPtr(size_t n) {
    return AlignedAllocator<T, Alignment>::allocate(n);
}

template<typename T, size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline std::unique_ptr<T, AlignedDeleter<T, Alignment>> createAlignedUniquePtr(size_t n) {
    return std::unique_ptr<T, AlignedDeleter<T, Alignment>>(AlignedAllocator<T, Alignment>::allocate(n));
}

template<typename T, size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline std::shared_ptr<T> createAlignedSharedPtr(size_t n) {
    return { AlignedAllocator<T, Alignment>::allocate(n),
             AlignedDeleter<T, Alignment>{} };
}

template<typename T, size_t Alignment = DefaultAlignment()>
using UniqueAlignedArray = std::unique_ptr<T[], AlignedDeleter<T, Alignment>>;

template<typename T, size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline UniqueAlignedArray<T, Alignment> createAlignedArray(size_t n) {
    return UniqueAlignedArray<T, Alignment>(AlignedAllocator<T, Alignment>::allocate(n));
}

template<typename T, size_t Alignment = DefaultAlignment()>
using AlignedVector = std::vector<T, AlignedAllocator<T, Alignment>>;

template<typename T, size_t Alignment = DefaultAlignment(), typename... Args>
[[nodiscard]] inline AlignedVector<T, Alignment> createAlignedVector(Args&&... args)
{
    return AlignedVector<T, Alignment>(std::forward<Args>(args)...);
}

} // namespace Stalker::Memory
