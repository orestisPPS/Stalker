#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include <Stalker/Core/Config/Alignment.h>

namespace Stalker::Memory {

using namespace Stalker::Core::Config;


template<typename T, std::size_t Alignment = alignof(T)>
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

    static T* allocate(std::size_t n) {
        if (n == 0) return nullptr;
        return static_cast<T*>(
            ::operator new(n * sizeof(T), std::align_val_t{Alignment})
        );                              // throws std::bad_alloc on failure
    }

    static void deallocate(T* p, std::size_t = 0) noexcept {
        ::operator delete(p, std::align_val_t{Alignment});
    }

    bool operator==(const AlignedAllocator&) const noexcept { return true; }
    bool operator!=(const AlignedAllocator&) const noexcept { return false; }
};


template<typename T, std::size_t Alignment = DefaultAlignment()>
struct AlignedDeleter {
    void operator()(T* p) const noexcept {
        AlignedAllocator<T, Alignment>::deallocate(p);
    }
};


template<typename T, std::size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline T* createAlignedRaw(std::size_t n) {
    return AlignedAllocator<T, Alignment>::allocate(n);
}

template<typename T, std::size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline std::unique_ptr<T, AlignedDeleter<T, Alignment>> createAlignedUnique(std::size_t n) {
    return std::unique_ptr<T, AlignedDeleter<T, Alignment>>(AlignedAllocator<T, Alignment>::allocate(n));
}

template<typename T, std::size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline std::shared_ptr<T> createAlignedShared(std::size_t n) {
    return { AlignedAllocator<T, Alignment>::allocate(n),
             AlignedDeleter<T, Alignment>{} };
}

template<typename T, std::size_t Alignment = DefaultAlignment()>
using UniqueAlignedArray = std::unique_ptr<T[], AlignedDeleter<T, Alignment>>;

template<typename T, std::size_t Alignment = DefaultAlignment()>
[[nodiscard]] inline UniqueAlignedArray<T, Alignment> createAlignedArray(std::size_t n) {
    return UniqueAlignedArray<T, Alignment>(AlignedAllocator<T, Alignment>::allocate(n));
}

template<typename T, std::size_t Alignment = DefaultAlignment()>
using AlignedVector = std::vector<T, AlignedAllocator<T, Alignment>>;

template<typename T, std::size_t Alignment = DefaultAlignment(), typename... Args>
[[nodiscard]] inline AlignedVector<T, Alignment> createAlignedVector(Args&&... args)
{
    return AlignedVector<T, Alignment>(std::forward<Args>(args)...);
}

} // namespace Stalker::Memory
