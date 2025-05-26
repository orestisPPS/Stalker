#ifndef SIMD_PERFORMANCETESTS_H
#define SIMD_PERFORMANCETESTS_H

#include "../../STLKR_TestBase.h"
#include "../../TestUtility.h"
// #include "../../../DataStructures/StalkerVector/AVX2/MemoryTraits.h"
#include <Stalker/Core/Units.h>
using namespace Stalker;
#include <Stalker/Mathematics/Operations/MathOperationsSIMD.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX2.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX512.h>
// #include "../../../StalkerMathematics/Operations/SIMD/SIMDMathOperations.h"
namespace STLKR_Tests {
    
    template<size_t Size = 32>
    class SIMD_PerformanceTests : public STLKR_TestBase {
    public:
        explicit SIMD_PerformanceTests() : STLKR_TestBase("SIMD Performance Tests VTEC KICKED IN YO") {
                setPath("SIMD_Logs", getPath("Tests") + "/Mathematics/Operations/logs/SIMD");
            }

       void runTest() override {


            for (size_t i = 0; i < 10; i++) {
                printSubtitle("Iteration: " + std::to_string(i), ColourType::PATSIOURA_RED);
                _logs.addParameter("size", std::to_string(Size));
                _logs.addParameter("unrollFactor", STALKER_UNROLL_FACTOR);
                _logs.addParameter("alignment", 64);
                _logs.addParameter("compiler flag", "o3");
                _testCopyAllTypes();
                _logs.exportToCSV(getPath("SIMD_Logs"), "SIMD_Performance");
                _logs.clearAllLogs();
            }

        }

    private:

        void _testCopyAllTypes(){
            printTitle("SIMD Copy Performance", "-", ColourType::WHITE);
            _testSIMDCopy<double>();
            _testSIMDCopy<float>();
            _testSIMDCopy<int>();
            // _testSIMDCopy<short>();
            // _testSIMDCopy<unsigned>();
            
        }

        template<typename T>
        void _testSIMDCopy(){
            //test classic copy
            T* source = new T[Size];
            T* destination = new T[Size];
            for (size_t i = 0; i < Size; i++)
                source[i] = static_cast<T>(i);
            auto unit = Core::TimeUnit::nanoseconds;
            //classic copy
            auto name = TestUtility::getTypeString<T>();
            _logs.startSingleObservationTimer("Classic Copy " + name, unit);
            _logs.startSingleObservationTimer("Classic Copy " + name, unit);
            for (size_t i = 0; i < Size; i++)
                destination[i] = source[i];
            _logs.stopSingleObservationTimer("Classic Copy " + name, unit);

            //memcpy copy
            _logs.startSingleObservationTimer("Memcpy Copy " + name, unit);
            std::memcpy(destination, source, sizeof(T) * Size);
            _logs.stopSingleObservationTimer("Memcpy Copy " + name, unit);

            delete[] source;
            delete[] destination;

            //SIMD copy
            T* simdSource = _createAlignedPtr<T>(Size, 64);
            T* simdDestination = _createEmptyAlignedPtr<T>(Size, 64);

            _logs.startSingleObservationTimer("SIMD Copy " + name, unit);
            Stalker::Memory::SIMD::MemoryOperationsSIMD<T, SIMDType::AVX512>::copy(simdSource, simdDestination, Size);
            _logs.stopSingleObservationTimer("SIMD Copy " + name, unit);
            _freeAlignedArray(simdSource);
            _freeAlignedArray(simdDestination);
        }

        template <typename T>
        static std::array<T, Size> _createAlignedArray(T value = 0) {
            std::array<T, Size> array;
            for (size_t i = 0; i < Size; i++){
                if (value == 0)
                    array[i] = static_cast<T>(i);
                else
                    array[i] = value;
            }
            return array;
        }
        
        template <typename T>
        static std::array<T, Size> _createEmptyAlignedArray() {
            alignas(64) std::array<T, Size> array;
            for (size_t i = 0; i < Size; i++){
                array[i] = 0;
            }
            return array;
        }


        template <typename T>
        static auto _createAlignedPtr(size_t size, size_t alignment, T value = 0) {
            T* ptr = static_cast<T*>(_mm_malloc(size * sizeof(T), alignment));
            for (size_t i = 0; i < size; i++) {
                ptr[i] = (value == 0) ? static_cast<T>(i) : value;
            }
            return ptr;
        }

        template <typename T>
        static auto _createEmptyAlignedPtr(size_t size, size_t alignment) {
            T* ptr = static_cast<T*>(_mm_malloc(size * sizeof(T), alignment));
            for (size_t i = 0; i < size; i++) {
                ptr[i] = 0;
            }
            return ptr;
        }

        template <typename T>
        static void _freeAlignedArray(T* ptr) {
            _mm_free(ptr);
        }
    };
}

#endif // SIMD_PERFORMANCETESTS_H