//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_ENUMERABLEBASE_H
#define STALKER_STLKR_ENUMERABLEBASE_H
#include "STLKR_SIMD/STLKR_SIMD_DataOperations.h"
#include "../../../ThreadingOperations/STLKR_ThreadingOperations.h"
#include <cstdio>

namespace STLKR_LinearAlgebra {
    template<typename T, size_t size, size_t threads = 1>
    class STLKR_EnumerableBase{
    public:




    protected:

        explicit STLKR_EnumerableBase(STLKR_SIMD simdPolicy){
            _dataPtr = nullptr;
            _simdPolicy = simdPolicy;
        }

        T* _dataPtr;
        STLKR_SIMD _simdPolicy;
        STLKR_ThreadingOperations<threads> _threading;
        STLKR_SIMD_DataOperations<T, size> _simdDataOperations;

        static_assert(size > 0, "Size must be greater than 0");
        static_assert(threads > 0, "Threads must be greater than 0");
        static_assert(threads > std::thread::hardware_concurrency(), "Threads must be greater than hardware concurrency");
        static_assert(std::type_index(typeid(T)) == std::type_index(typeid(float)) ||
                      std::type_index(typeid(T)) == std::type_index(typeid(double)) ||
                      std::type_index(typeid(T)) == std::type_index(typeid(int)) ||
                      std::type_index(typeid(T)) == std::type_index(typeid(short)) ||
                      std::type_index(typeid(T)) == std::type_index(typeid(unsigned)), "Supported types are float, double, int, short, unsigned");

    };
}
#endif //STALKER_STLKR_ENUMERABLEBASE_H
