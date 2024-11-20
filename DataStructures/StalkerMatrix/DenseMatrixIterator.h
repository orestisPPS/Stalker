#ifndef DENSEMATRIXITERATOR_H
#define DENSEMATRIXITERATOR_H

#include "MatrixTypes.h"
#include "../Iterators/StrideIterator.h"

template <typename Derived, typename T,  MatrixSubstructureType SubstructureT, OrderType OrderT>
class MatrixIteratorBase : public DynamicStrideIteratorBase<Derived<T>, T> {
    using Base = DynamicStrideIteratorBase<Derived<T>, T>;
public:
    inline std::ptrdiff_t _stride(std::size_t index) const { return child()._stride(index); }
private:
    inline Derived& child() { return *static_cast<Derived*>(this); }
    inline const Derived& child() const { return *static_cast<const Derived*>(this); }
protected:
    MatrixIteratorBase(T* startPtr) : Base(startPtr, 0){}
    MatrixIteratorBase(const T* startPtr) : Base(const_cast<T*>(startPtr) {}
}; 

template <typename T, MatrixSubstructureType SubstructureT, OrderType OrderT>
class DenseMatrixIterator : public MatrixIteratorBase<T, SubstructureT, OrderT> {
    using Base = MatrixIteratorBase<Derived, T, SubstructureT, OrderT>;
public:
     inline std::ptrdiff_t _stride(std::size_t index) const { return child()._stride(index); }

protected:
    DenseMatrixIteratorBase(T* startPtr) : Base(startPtr, 0){}
    DenseMatrixIteratorBase(const T* startPtr) : Base(const_cast<T*>(startPtr) {}
};


#endif // DENSEMATRIXITERATOR_H



// template <typename T>
// class UpperTriangularMatrixIterator : public DynamicStrideIterator<UpperTriangularMatrixIterator<T>, T> {
//     using Base = DynamicStrideIterator<UpperTriangularMatrixIterator<T>, T>;

// public:
//     UpperTriangularMatrixIterator(T* startPtr, std::size_t rows)
//         : Base(startPtr, 0), _rows(rows) {}

//     UpperTriangularMatrixIterator(const T* startPtr, std::size_t rows)
//         : Base(const_cast<T*>(startPtr), 0), _rows(rows) {}

//     inline std::ptrdiff_t _stride(std::size_t index) const {
//         return _rows - index;
//     }
// };