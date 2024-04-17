//
// Created by hal9000 on 4/17/24.
//

#ifndef STALKER_STLKR_ENUMERABLEOPERATIONSBASE_H
#define STALKER_STLKR_ENUMERABLEOPERATIONSBASE_H
#include "../../ThreadingOperations/STLKR_ThreadingOperations.h"
namespace STLKR_LinearAlgebra {
    template <unsigned size, typename T, unsigned availableThreads = 1>
    class STLKR_EnumerableOperationsBase {
        
    public:
        explicit STLKR_EnumerableOperationsBase(T* data) : _data(data) {}
        
        STLKR_ThreadingOperations<availableThreads> _threadingOperations;
        
    private:
        T* _data;
    };
}
#endif //STALKER_STLKR_ENUMERABLEOPERATIONSBASE_H
