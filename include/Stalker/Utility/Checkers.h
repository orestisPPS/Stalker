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

#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <string>
#include <stdexcept>
#include <Stalker/Utility/Printers.h>

namespace Stalker::Utility {

static inline void checkSize(const std::size_t size, const std::size_t expectedSize, const std::string& message = "") {
    if (size != expectedSize)
        message.empty() ? throw std::invalid_argument(errorMessage("Provided size (" + std::to_string(size) + ") does not match the expected size (" + std::to_string(expectedSize) + ")."))
                        : throw std::invalid_argument(errorMessage(message + ": Provided size (" + std::to_string(size) + ") does not match the expected size (" + std::to_string(expectedSize) + ")."));
}

static inline void checkPointer(const void* ptr, const std::string& message = "") {
    if (!ptr)
        message.empty() ? throw std::invalid_argument(errorMessage("Null Pointer."))
                        : throw std::invalid_argument(errorMessage(message + ": Null Pointer."));
}

template <typename T, typename U>
static inline void checkSameType(std::string message = "") {
    if (!std::is_same<T, U>::value)
        message.empty() ? throw std::invalid_argument(errorMessage("The types do not match."))
                        : throw std::invalid_argument(errorMessage(message + ": The types do not match."));
}

template <typename T>
static inline void checkRange(const T& value, const T& min, const T& max, const std::string& message = "") {
    if (value < min || value > max)
        message.empty() ? throw std::out_of_range(errorMessage("Value (" + std::to_string(value) + ") is out of the acceptable range. Minimum: " + std::to_string(min) + ", Maximum: " + std::to_string(max) + "."))
                        : throw std::out_of_range(errorMessage(message + ": Value (" + std::to_string(value) + ") is out of the acceptable range. Minimum: " + std::to_string(min) + ", Maximum: " + std::to_string(max) + "."));
}

static inline void checkIndex(const std::size_t index, const std::size_t size, const std::string& message = "") {
    if (index >= size)
        message.empty() ? throw std::out_of_range(errorMessage("Index (" + std::to_string(index) + ") is out of bounds. The size is " + std::to_string(size) + "."))
                        : throw std::out_of_range(errorMessage(message + ": Index (" + std::to_string(index) + ") is out of bounds. The size is " + std::to_string(size) + "."));
}

static inline void checkMatrixIndeces(const std::size_t row, const std::size_t col, const std::size_t rows, const std::size_t cols, const std::string& message = "") {
    if (row >= rows || col >= cols)
        message.empty() ? throw std::out_of_range(errorMessage("Matrix index (" + std::to_string(row) + ", " + std::to_string(col) + ") is out of bounds. The matrix is " + std::to_string(rows) + "x" + std::to_string(cols) + "."))
                        : throw std::out_of_range(errorMessage(message + ": Matrix index (" + std::to_string(row) + ", " + std::to_string(col) + ") is out of bounds. The matrix is " + std::to_string(rows) + "x" + std::to_string(cols) + "."));
}

static inline void checkSquareMatrix(const std::size_t rows, const std::size_t cols, const std::string& message = "") {
    if (rows != cols)
        message.empty() ? throw std::invalid_argument(errorMessage("Matrix not square. It has " + std::to_string(rows) + " rows and " + std::to_string(cols) + " columns."))
                        : throw std::invalid_argument(errorMessage(message + ": Matrix not square. It has " + std::to_string(rows) + " rows and " + std::to_string(cols) + " columns."));
}

} // namespace Stalker::Utility