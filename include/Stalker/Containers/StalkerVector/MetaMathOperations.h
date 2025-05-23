//
// Created by hal9000 on 9/2/24.
//

#ifndef STALKER_METAMATHOPERATIONS_H
#define STALKER_METAMATHOPERATIONS_H


class MetaMathOperations {
public:
    template <typename T, unsigned Power>
    static constexpr inline T power(const T &base) {
        if constexpr (Power == 0) {
            return 1;
        }
        else if constexpr (Power == 1) {
            return base;
        }
        else if constexpr (Power % 2 == 0) { 
            return power<T, Power / 2>(base * base);
        }
        else {
            return base * power<T, Power - 1>(base);
        }
    }
    
    template <typename T, unsigned size, unsigned Power = 1>
    static constexpr inline T sumOfPower(const T* data) {
        if constexpr (size == 0) {
            return 0;
        }
        else {
            return power<T, Power>(data[size - 1]) + sumOfPower<T, size - 1, Power>(data);
        }
    }

    template <typename T, unsigned size>
    static constexpr inline void add(const T* data1, const T* data2, T* result, T scalar1 = 1, T scalar2 = 1) {
        if constexpr (size > 0){
            *(result + size - 1) = *(data1 + size - 1) * scalar1 + *(data2 + size - 1) * scalar2; 
            add<T, size - 1>(data1, data2, result, scalar1, scalar2);
        }
        else return;
    }
    
    template <typename T, unsigned size>
    static constexpr inline void subtract(const T* data1, const T* data2, T* result, T scalar1 = 1, T scalar2 = 1) {
        if constexpr (size > 0){
            *(result + size - 1) = *(data1 + size - 1) * scalar1 - *(data2 + size - 1) * scalar2; 
            subtract<T, size - 1>(data1, data2, result, scalar1, scalar2);
        }
        else return;
    }
    
    template <typename T, unsigned size>
    static constexpr inline void multiply(const T* data1, const T* data2, T* result, T scalar1 = 1, T scalar2 = 1) {
        if constexpr (size > 0){
            *(result + size - 1) = *(data1 + size - 1) * scalar1 * *(data2 + size - 1) * scalar2; 
            multiply<T, size - 1>(data1, data2, result, scalar1, scalar2);
        }
        else return;
    }
    

    template<typename T, unsigned size>
    static constexpr inline T dotProduct(const T* data) {
        if constexpr (size == 0) {
            return 1;
        }
        else {
            return data[size - 1] * dotProduct<T, size - 1>(data);
        }
    }
    
    template<typename T, unsigned size>
    static constexpr inline T factorial() {
        if constexpr (size == 0) {
            return 1;
        }
        else {
            return size * factorial<T, size - 1>();
        }
    }
    
    template<typename T, unsigned size>
    static constexpr inline T fibonacci() {
        if constexpr (size == 0) {
            return 0;
        }
        else if constexpr (size == 1) {
            return 1;
        }
        else {
            return fibonacci<T, size - 1>() + fibonacci<T, size - 2>();
        }
    }
    
private:
    
};

#endif //STALKER_METAMATHOPERATIONS_H
