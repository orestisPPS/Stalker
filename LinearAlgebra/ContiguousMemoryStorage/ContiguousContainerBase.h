//
// Created by hal9000 on 3/11/24.
//

#ifndef STALKER_CONTIGUOUSCONTAINERBASE_H
#define STALKER_CONTIGUOUSCONTAINERBASE_H

namespace LinearAlgebra::ContiguousMemoryStorage
{
    template <typename T>
    class ContiguousContainerBase
    {
    public:
        
    private:
        
        T* _data;
    
        unsigned _size;
        
        unsigned _threads;
    };
}

#endif //STALKER_CONTIGUOUSCONTAINERBASE_H
