/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <cstddef>
#include <Stalker/Mathematics/Vector/VectorMath.h>


namespace Stalker::Mathematics {

    struct MatrixMath {
        
    public:
        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(size_t nRows, size_t nCols, const T* matA, const T * matB, T* matRes) {
            VectorMath::add<T, Trait>(nRows * nCols, matA, matB, matRes);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(size_t nRows, size_t nCols, const T* matA, const T * matB, T* matRes, T scalarA, T scalarB) {
            VectorMath::add<T, Trait>(nRows * nCols, matA, matB, matRes, scalarA, scalarB);
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                         size_t strideA, size_t strideB, size_t strideResult) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::add<T, Trait>(sizeD2, matA, matB, matRes);
            }
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                     size_t strideA, size_t strideB, size_t strideResult, T scalarA, T scalarB) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::add<T, Trait>(sizeD2, matA, matB, matRes, scalarA, scalarB);
            }
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(size_t nRows, size_t nCols, const T* matA, const T * matB, T* matRes) {
            VectorMath::subtract<T, Trait>(nRows * nCols, matA, matB, matRes);
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(size_t nRows, size_t nCols, const T* matA, const T * matB, T* matRes, T scalarA, T scalarB) {
            VectorMath::subtract<T, Trait>(nRows * nCols, matA, matB, matRes, scalarA, scalarB);
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                        size_t strideA, size_t strideB, size_t strideResult) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::subtract<T, Trait>(sizeD2, matA, matB, matRes);
            }
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                     size_t strideA, size_t strideB, size_t strideResult, T scalarA, T scalarB) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::subtract<T, Trait>(sizeD2, matA, matB, matRes, scalarA, scalarB);
            }
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void multiply(size_t nRows, size_t nCols, const T* matA, const T * matB, T* matRes) {
            VectorMath::multiply<T, Trait>(nRows * nCols, matA, matB, matRes);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void multiply(size_t nRows, size_t nCols, const T* matA, const T * matB, T* matRes, T scalarA, T scalarB) {
            VectorMath::multiply<T, Trait>(nRows * nCols, matA, matB, matRes, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline T sum(size_t nRows, size_t nCols, const T* matA) {
            return VectorMath::sum<T, Trait>(nRows * nCols, matA);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void scale(size_t nRows, size_t nCols, T* matA, T scalar) {
            return VectorMath::scale<T, Trait>(nRows * nCols, matA, scalar);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void scale(size_t nRows, size_t nCols, const T* matA, T* matRes, T scalar) {
            return VectorMath::scale<T, Trait>(nRows * nCols, matA, matRes, scalar);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void addConstant(size_t nRows, size_t nCols, T* matA, T constant) {
            return VectorMath::addConstant<T, Trait>(nRows * nCols, matA, constant);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        static constexpr inline void addConstant(size_t nRows, size_t nCols, const T* matA, T* matRes, T constant) {
            return VectorMath::addConstant<T, Trait>(nRows * nCols, matA, matRes, constant);
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void matrixVectorMultiply(size_t nRows, size_t nCols, const T* A, const T* x, T* y, size_t strideA) {
            if constexpr (RowMajor) {
                for (size_t i = 0; i < nRows; ++i) {
                    y[i] = VectorMath::dot<T, Trait>(nCols, A + i * strideA, x);
                }
            } else {
                MemoryOperations::setValue<T, Trait>(nRows, y, T(0));
                for (size_t j = 0; j < nCols; ++j) {
                    VectorMath::axpy<T, Trait>(nRows, A + j * strideA, y, y, x[j]);
                }
            }
        }
        
        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void matrixVectorMultiply(size_t nRows, size_t nCols, const T* A, const T* x, T* y, size_t strideA, T alpha, T beta) {
            if constexpr (RowMajor) {
                for (size_t i = 0; i < nRows; ++i) {
                    y[i] = alpha * VectorMath::dot<T, Trait>(nCols, A + i * strideA, x) + beta * y[i];
                }
            } else {
                if (beta == T(0)) {
                    MemoryOperations::SetValue<T, Trait>(nRows, y, T(0));
                } else if (beta != T(1)) {
                    VectorMath::scale<T, Trait>(nRows, y, beta);
                }
                for (size_t j = 0; j < nCols; ++j) {
                    VectorMath::axpy<T, Trait>(nRows, A + j * strideA, y, y, x[j] * alpha);
                }
            }
        }

        template <typename T, bool RowMajor, typename Trait = DefaultExecutionTrait>
        static constexpr inline void vectorMatrixMultiply(size_t nRows, size_t nCols, const T* x, const T* A, T* y, size_t strideA) {
            if constexpr (RowMajor) {
                MemoryOperations::SetValue<T, Trait>(nCols, y, T(0));
                const T* endA = A + nRows * strideA;
                for (; A != endA; A += strideA, ++x) {
                    VectorMath::axpy<T, Trait>(nCols, A, y, y, *x);
                }
            } else {
                const T* endA = A + nCols * strideA;
                for (; A != endA; A += strideA, ++y) {
                    *y = VectorMath::dot<T, Trait>(nRows, A, x);
                }
            }
        }

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
   

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes) {
            VectorMath::add<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, matB, matRes);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes, T scalarA, T scalarB) {
            VectorMath::add<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, matB, matRes, scalarA, scalarB);
        }

        template<typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                         size_t strideA, size_t strideB, size_t strideResult) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::add<T, ThreadTrait, Trait>(threadTrait, sizeD2, matA, matB, matRes);
            }
        }

        template<typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void add(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                         size_t strideA, size_t strideB, size_t strideResult, T scalarA, T scalarB) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::add<T, ThreadTrait, Trait>(threadTrait, sizeD2, matA, matB, matRes, scalarA, scalarB);
            }
        }

        template<typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes) {
            VectorMath::subtract<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, matB, matRes);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes, T scalarA, T scalarB) {
            VectorMath::subtract<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, matB, matRes, scalarA, scalarB);
        }

        template<typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                         size_t strideA, size_t strideB, size_t strideResult) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::subtract<T, ThreadTrait, Trait>(threadTrait, sizeD2, matA, matB, matRes);
            }
        }

        template<typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void subtract(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes,
                                         size_t strideA, size_t strideB, size_t strideResult, T scalarA, T scalarB) {
            size_t sizeD1, sizeD2;
            _getSizes<RowMajor>(nRows, nCols, sizeD1, sizeD2);
            const T* endA = matA + sizeD1 * strideA;
            for (; matA != endA; matA += strideA, matB += strideB, matRes += strideResult) {
                VectorMath::subtract<T, ThreadTrait, Trait>(threadTrait, sizeD2, matA, matB, matRes, scalarA, scalarB);
            }
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void multiply(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes) {
            VectorMath::multiply<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, matB, matRes);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void multiply(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA, const T* matB, T* matRes, T scalarA, T scalarB) {
            VectorMath::multiply<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, matB, matRes, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline T sum(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* matA) {
            return VectorMath::sum<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void scale(ThreadTrait& threadTrait, size_t nRows, size_t nCols, T* matA, T scalar) {
            VectorMath::scale<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, scalar);
        }
        
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void addConstant(ThreadTrait& threadTrait, size_t nRows, size_t nCols, T* matA, T constant) {
            VectorMath::addConstant<T, ThreadTrait, Trait>(threadTrait, nRows * nCols, matA, constant);
        }

        template <typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void matrixVectorMultiply(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* A, const T* x, T* y, size_t strideA) {
            if constexpr (RowMajor) {
                const T* endA = A + nRows * strideA;
                for (; A != endA; A += strideA, ++y) {
                    *y = VectorMath::dot<T, ThreadTrait, Trait>(threadTrait, nCols, A, x);
                }
            } else {
                VectorMath::scale<T, ThreadTrait, Trait>(threadTrait, nRows, y, T(0));
                const T* endA = A + nCols * strideA;
                for (; A != endA; A += strideA, ++x) {
                    VectorMath::axpy<T, ThreadTrait, Trait>(threadTrait, nRows, A, y, y, *x);
                }
            }
        }
        
        template <typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void matrixVectorMultiply(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* A, const T* x, T* y, size_t strideA, T alpha, T beta) {
            if constexpr (RowMajor) {
                const T* endA = A + nRows * strideA;
                for (; A != endA; A += strideA, ++y) {
                    *y = alpha * VectorMath::dot<T, ThreadTrait, Trait>(threadTrait, nCols, A, x) + beta * (*y);
                }
            } else {
                if (beta == T(0)) {
                    VectorMath::scale<T, ThreadTrait, Trait>(threadTrait, nRows, y, T(0));
                } else if (beta != T(1)) {
                    VectorMath::scale<T, ThreadTrait, Trait>(threadTrait, nRows, y, beta);
                }
                
                const T* endA = A + nCols * strideA;
                for (; A != endA; A += strideA, ++x) {
                    VectorMath::axpy<T, ThreadTrait, Trait>(threadTrait, nRows, A, y, y, (*x) * alpha);
                }
            }
        }

        template <typename T, bool RowMajor, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        static constexpr inline void vectorMatrixMultiply(ThreadTrait& threadTrait, size_t nRows, size_t nCols, const T* x, const T* A, T* y, size_t strideA) {
            if constexpr (RowMajor) {
                VectorMath::scale<T, ThreadTrait, Trait>(threadTrait, nCols, y, T(0));
                const T* endA = A + nRows * strideA;
                for (; A != endA; A += strideA, ++x) {
                    VectorMath::axpy<T, ThreadTrait, Trait>(threadTrait, nCols, A, y, y, *x);
                }
            } else {
                const T* endA = A + nCols * strideA;
                for (; A != endA; A += strideA, ++y) {
                    *y = VectorMath::dot<T, ThreadTrait, Trait>(threadTrait, nRows, A, x);
                }
            }
        }

        #endif
    private:

        template <bool RowMajor>
        static constexpr void _getSizes(size_t nRows, size_t nCols, size_t& sizeD1, size_t& sizeD2) {
            if constexpr (RowMajor) {
                sizeD1 = nRows;
                sizeD2 = nCols;
            } else {
                sizeD1 = nCols;
                sizeD2 = nRows;
            }
        }
    };

} // namespace Stalker::Mathematics