/*
 * Example: STAΛKER API Showcase
 * Demonstrating Memory Operations and Vector Math Subsystems.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <Stalker/Mathematics/Vector/VectorMath.h>
#include <Stalker/Memory/Allocators.h>
#include <Stalker/Memory/MemoryOperations.h>
#include <Stalker/Mathematics/Random.h>
#include <assert.h>

using namespace Stalker::Core;
using namespace Stalker::Core::Config;
using namespace Stalker::Mathematics;
using namespace Stalker::Memory;

// res = alpha * x + y
void exampleAxpySIMD() {
    size_t size = 1'000'000;
    auto x = Stalker::Memory::createAlignedVector<int, 64>(size, 1.0);
    auto y = Stalker::Memory::createAlignedVector<int, 64>(size, 2.0);
    auto result = Stalker::Memory::createAlignedVector<int, 64>(size);
    int alpha = 3.0;
    auto expected = std::vector<int>(size, 5.0); // 1.0 * 3.0 + 2.0 = 5.0

    constexpr T_SIMD          InstructionSet = T_SIMD::AVX2;
    constexpr bool            IsAligned      = true;
    constexpr size_t          UnrollFactor   = 2;
    constexpr T_SIMDStore     StorePolicy    = T_SIMDStore::Cached;
    constexpr T_PrefetchHints PrefetchPolicy = T_PrefetchHints::HintT0;
    constexpr T_ILPPolicy     ILPPolicy      = T_ILPPolicy::Grouped;

    using Trait = ExecutionTraitSIMD<InstructionSet, IsAligned, StorePolicy, UnrollFactor, PrefetchPolicy, ILPPolicy>;
    VectorMath::axpy<int, Trait>(size, x.data(), y.data(), result.data(), alpha);

    assert(std::equal(result.begin(), result.end(), expected.begin()));
    std::cout << "SIMD AXPY successful and verified for " << size << " elements." << std::endl;
}

void exampleCopySIMD() {
    size_t size = 1024;
    auto a = Stalker::Memory::createAlignedVector<double, 32>(size);
    auto b = Stalker::Memory::createAlignedVector<double, 32>(size);

    Stalker::Mathematics::Random::uniform<double>(size, a.data(), 0, 10);

    using Trait = ExecutionTraitSIMD<>;

    Stalker::Memory::MemoryOperations::copy<double, Trait>(size, a.data(), b.data());

    assert(std::equal(a.begin(), a.end(), b.begin()));
    std::cout << "SIMD Copy successful and verified for " << size << " elements." << std::endl;
}

void exampleDotProductScalar() {
    size_t size = 1'000;
    auto a = Stalker::Memory::createAlignedVector<double, 64>(size, 1.0);
    auto b = Stalker::Memory::createAlignedVector<double, 64>(size, 2.0);

    using Trait = ExecutionTraitScalar<false>;
    [[maybe_unused]] double res = VectorMath::dot<double, Trait>(size, a.data(), b.data());

    assert(res == size * 1.0 * 2.0);
    std::cout << "Scalar Dot Product successful and verified for " << size << " elements." << std::endl;
}

void exampleSetValueScalarSTL() {
    const size_t size = 1024;
    const double value = 3.14;
    auto a = std::vector<double>(size);
    auto expected = std::vector<double>(size, value);

    Random::uniform<double>(size, a.data(), 0, 10);

    constexpr bool IsSTL = true;
    using Trait = ExecutionTraitScalar<IsSTL>;
    Stalker::Memory::MemoryOperations::setValue<double, Trait>(size, a.data(), value);

    assert(std::equal(a.begin(), a.end(), expected.begin()));
    std::cout << "Scalar STL SetValue successful and verified for " << size << " elements." << std::endl;
}

void exampleSumConstexprUnrolled() {
    constexpr size_t size = 200;
    constexpr double value = 1.0;
    
    constexpr auto a = []() {
        std::array<double, size> array{};
        for (size_t i = 0; i < size; ++i)
            array[i] = value;
        return array;
    }();

    constexpr size_t UnrollFactor = 50;
    using Trait = ExecutionTraitUnrolled<UnrollFactor>;
    constexpr double sum = VectorMath::sum<double, Trait>(size, a.data());

    static_assert(sum == size * value, "Constexpr Unrolled Sum failed");
    std::cout << "Constexpr Unrolled Sum successful and verified for " << size << " elements." << std::endl;
}

void exampleScaleFullyUnrolled() {
    constexpr size_t size = 200;
    const double value = 1.0;
    const double scalar = 3.14;
    
    auto a = std::vector<double>(size, value);
    auto expected = std::vector<double>(size, value * scalar);

    using Trait = ExecutionTraitUnrolled<size>;
    VectorMath::scale<double, Trait>(size, a.data(), scalar);

    assert(std::equal(a.begin(), a.end(), expected.begin()));
    std::cout << "Unrolled Scale successful and verified for " << size << " elements." << std::endl;
}


int main() {
    exampleAxpySIMD();
    exampleCopySIMD();
    exampleSetValueScalarSTL();
    exampleDotProductScalar();
    exampleSumConstexprUnrolled();
    exampleScaleFullyUnrolled();

    return 0;
}
