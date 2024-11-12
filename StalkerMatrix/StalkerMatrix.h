#ifndef STALKER_MATRIX_H
#define STALKER_MATRIX_H
#include <type_traits>
#include "MatrixTypes.h"
    
    template<typename T>
    class StalkerMatrix {
        public:
        static_assert(std::is_same<T, double>::value || std::is_same<T, int>::value ||
                      std::is_same<T, unsigned>::value || std::is_same<T, short>::value ||
                      std::is_same<T, float>::value, "StalkerMatrix only supports double, int, unsigned, short, and float types.");

                      
            StalkerMatrix(unsigned rows, unsigned columns, MatrixStorageType storageType = Dense, FormType FormT = NonSymmetric, OrderType OrderT = RowMajor) :
                          _rows(rows), _columns(columns), _order(order), _storageType(storageType), _FormT(FormT), _order(OrderT) {}

            T& operator()(unsigned row, unsigned col) {

            }

            const T& operator()(unsigned row, unsigned col) const {

            }
            
            unsigned getNumRows() const { return _rows; }
            unsigned getNumColumns() const { return _columns; }
            OrderType getElementsOrderType() const { return _order; }
            MatrixStorageType getStorageType() const { return _storageType; }
            FormType getFormType() const { return _FormT; }


        protected:
            unsigned _rows;
            unsigned _columns;
            MatrixStorageType _storageType;
            FormType _FormT;
            OrderType _order;

    };
#endif // STALKER_MATRIX_H