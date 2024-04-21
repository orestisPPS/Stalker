//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_ENUMERABLEBASE_H
#define STALKER_STLKR_ENUMERABLEBASE_H


namespace STLKR_LinearAlgebra {
    template <typename T, unsigned threads = 1>
    class EnumerableBase {
    public:
        
    protected:
        T* _data;
        
        unsigned _size;
    };
}
#endif //STALKER_STLKR_ENUMERABLEBASE_H
