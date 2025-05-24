#include <cstring>
#include <bit>

namespace Stalker::Memory {

/**
 * @class MemoryManagement
 * @tparam T Managed type (must be complete at instantiation)
 * 
 * @brief Type-aware memory manager implementing C++ Core Guidelines for resource management
 * 
 * ## Core Principles
 * 1. **Type-Driven Optimization**: Operations dispatch based on T's triviality
 * 2. **Alignment Safety**: Power-of-two guarantees (ISO C++ [basic.align]/5)
 * 3. **Lifetime Separation**: Strict RAII phase separation (Allocation ≠ Construction)
 * 4. **Zero-Overhead**: Trivial types use optimal C-style operations
 * 
 * @invariant All operations maintain:
 * - Alignment ≥ alignof(T)
 * - Non-overlapping memory regions (unless explicitly allowed)
 * 
 * @example
 * // SIMD-optimized allocation
 * float* data = MemoryManagement<float>::allocate<64>(1000); // 64B alignment
 * MemoryManagement<float>::construct_array(data, 1000);
 */
template <typename T>
struct MemoryManagement {
    /**
     * @brief Allocate memory with natural type alignment
     * @param elements Number of contiguous T elements
     * @return Pointer to raw storage (uninitialized)
     * @throws Nothing (noexcept)
     * 
     * @details
     * **Mathematical Model**:
     * - Allocates S = elements × sizeof(T) bytes
     * - Alignment A = alignof(T)
     * - Guarantees: ∀ptr ∈ return, ptr ≡ 0 mod A
     * 
     * @complexity O(1) with modern allocators
     */
    [[nodiscard]] static T* allocate(size_t elements) noexcept {
        return static_cast<T*>(::operator new(elements * sizeof(T)));
    }

    /**
     * @brief Over-aligned type-safe allocation
     * @tparam Align Required alignment (must satisfy Align > alignof(T))
     * @param elements Element count
     * @return Pointer meeting alignment constraint
     * 
     * @details
     * **Alignment Protocol**:
     * 1. Compute A' = 2⌈log₂(Align)⌉ (via std::bit_ceil)
     * 2. Verify A' > alignof(T) (SFINAE constraint)
     * 3. Allocate via aligned operator new (C++17 [new.delete.array])
     * 
     * @throws std::bad_alloc if alignment not power-of-two
     */
    template <size_t Align = alignof(T)>
    [[nodiscard]] static T* allocate(size_t elements) requires(Align > alignof(T)) {
        constexpr size_t alignment = std::bit_ceil(Align);
        void* ptr = ::operator new(elements * sizeof(T), std::align_val_t{alignment});
        return static_cast<T*>(ptr);
    }

    /**
     * @brief Low-level byte allocation
     * @param bytes Number of raw bytes
     * @param alignment Required power-of-two alignment
     * @return void* to untyped storage
     * @throws std::bad_alloc for invalid alignment
     * 
     * @remark
     * **max_align_t Rationale**:
     * Default alignment matches maximum fundamental alignment:
     * alignof(std::max_align_t) = max{ alignof(scalar types) }
     * 
     * @warning Not type-safe - prefer typed allocate()
     */
    [[nodiscard]] static void* allocate_bytes(size_t bytes, size_t alignment = alignof(::max_align_t)) {
        if ((alignment & (alignment - 1)) != 0) 
            throw std::bad_alloc();
        
        void* ptr = ::operator new(bytes, std::align_val_t{alignment});
        if (reinterpret_cast<uintptr_t>(ptr) % alignment != 0)
            throw std::bad_alloc();
        
        return ptr;
    }

    /**
     * @brief Deallocate memory with proper alignment
     * @param ptr Previously allocated pointer
     * 
     * @post ptr invalidated (dangling)
     * @note Matches operator delete to allocation method:
     * - Regular delete for default alignment
     * - Aligned delete for over-aligned
     */
    static void deallocate(T* ptr) noexcept {
        ::operator delete(ptr);
    }

    /**
     * @brief Alignment-specific deallocation
     * @tparam Align Original allocation alignment
     * 
     * @pre Must match allocation alignment
     * @see ISO C++ [expr.delete]/3
     */
    template <size_t Align>
    static void deallocate(T* ptr) noexcept requires(Align > alignof(T)) {
        ::operator delete(ptr, std::align_val_t{Align});
    }

    /**
     * @brief Perfect-forwarding construction
     * @tparam Args Constructor argument types
     * @param ptr Raw memory location
     * @param args Constructor arguments
     * 
     * @effect Constructs T via placement new:
     * new (ptr) T(std::forward<Args>(args)...)
     * 
     * @throws Propagates T's constructor exceptions
     */
    template <typename... Args>
    static void construct(T* ptr, Args&&... args) 
        noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        new (ptr) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Batch default construction
     * @param begin Start of raw memory array
     * @param count Number of elements
     * 
     * @requires T must be default constructible
     * @note Non-trivial types get proper initialization
     */
    static void construct_array(T* begin, size_t count) 
        noexcept(std::is_nothrow_default_constructible_v<T>) {
        for (size_t i = 0; i < count; ++i) {
            new (begin + i) T();
        }
    }

    /**
     * @brief Conditional destruction
     * @param ptr Object to destroy
     * 
     * @effect If !is_trivially_destructible_v<T>:
     *   Calls ~T(), otherwise no-op
     */
    static void destroy(T* ptr) noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            ptr->~T();
        }
    }

    /**
     * @brief Array destruction
     * @param begin Start of object array
     * @param count Number of elements
     * 
     * @post All objects in [begin, begin+count) destroyed
     */
    static void destroy_array(T* begin, size_t count) noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (size_t i = 0; i < count; ++i) {
                (begin + i)->~T();
            }
        }
    }

    /**
     * @brief Type-dispatched memory relocation
     * @param dest Destination (uninitialized)
     * @param src Source (valid objects)
     * @param count Number of elements
     * 
     * @algorithm
     * - Trivial: memmove (bitwise copy)
     * - Non-trivial: Move construct + destroy source
     * 
     * @post src contains moved-from objects if non-trivial
     */
    static void relocate(T* dest, T* src, size_t count) 
        noexcept(std::is_nothrow_move_constructible_v<T>) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memmove(dest, src, count * sizeof(T));
        } else {
            for (size_t i = 0; i < count; ++i) {
                new (dest + i) T(std::move(src[i]));
                destroy(src + i);
            }
        }
    }

    /**
     * @brief Type-optimized copy
     * @param src Source data
     * @param dest Destination (uninitialized)
     * @param count Element count
     * 
     * @requires T must be copy constructible
     * @effect For each i ∈ [0, count):
     *   new (dest+i) T(src[i])
     */
    template <typename U = T>
    static void copy(const T* src, T* dest, size_t count) 
        noexcept(std::is_nothrow_copy_constructible_v<T>) 
        requires(std::is_copy_constructible_v<U>) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memcpy(dest, src, count * sizeof(T));
        } else {
            for (size_t i = 0; i < count; ++i) {
                new (dest + i) T(src[i]);
            }
        }
    }

    /**
     * @brief Type-aware memory initialization
     * @param dest Memory region to initialize
     * @param value Initialization value
     * @param count Number of elements
     * 
     * @optimization For char-like types:
     *   Uses memset (O(1) per byte vs O(n) per element)
     */
    static void set(T* dest, const T& value, size_t count) {
        if constexpr (std::is_trivially_copyable_v<T> && sizeof(T) == 1) {
            ::memset(dest, static_cast<int>(value), count);
        } else {
            for (size_t i = 0; i < count; ++i) {
                new (dest + i) T(value);
            }
        }
    }

    /**
     * @brief Alignment verification
     * @param ptr Memory address
     * @param alignment Expected power-of-two alignment
     * @return true iff ptr ≡ 0 mod alignment
     * 
     * @formula (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0
     */
    static bool is_aligned(const T* ptr, size_t alignment = alignof(T)) noexcept {
        return (reinterpret_cast<uintptr_t>(ptr) % alignment) == 0;
    }

    /**
     * @brief Safe reallocation with growth/shrink
     * @param old_ptr Existing allocation
     * @param old_size Current element count
     * @param new_size Desired element count
     * @return New allocation with copied/moved data
     * 
     * @procedure
     * 1. Allocate new_size elements
     * 2. Relocate/copy min(old_size, new_size) elements
     * 3. Deallocate old_ptr
     * 
     * @throws std::bad_alloc on allocation failure (strong guarantee)
     */
    [[nodiscard]] static T* reallocate(T* old_ptr, size_t old_size, size_t new_size) {
        T* new_ptr = allocate(new_size);
        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            relocate(new_ptr, old_ptr, std::min(old_size, new_size));
        } else {
            copy(old_ptr, new_ptr, std::min(old_size, new_size));
        }
        deallocate(old_ptr);
        return new_ptr;
    }
};

} // namespace Stalker::Memory