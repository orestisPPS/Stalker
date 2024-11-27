#ifndef CHECKERS_H
#define CHECKERS_H

#include <type_traits>
#include <iostream>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include "Printers.h"


static inline void checkSize(const std::size_t size, const std::size_t expectedSize, const std::string& message = "") {
    if (size != expectedSize)
        message.empty() ? throw std::invalid_argument(Printers::errorMessage("Provided size (" + std::to_string(size) + ") does not match the expected size (" + std::to_string(expectedSize) + ")."))
                        : throw std::invalid_argument(Printers::errorMessage(message + ": Provided size (" + std::to_string(size) + ") does not match the expected size (" + std::to_string(expectedSize) + ")."));
}

static inline void checkPointer(const void* ptr, const std::string& message = "") {
    if (!ptr)
        message.empty() ? throw std::invalid_argument(Printers::errorMessage("Null Pointer."))
                        : throw std::invalid_argument(Printers::errorMessage(message + ": Null Pointer."));
}

template <typename T, typename U>
static inline void checkSameType(std::string message = "") {
    if (!std::is_same<T, U>::value)
        message.empty() ? throw std::invalid_argument(Printers::errorMessage("The types do not match."))
                        : throw std::invalid_argument(Printers::errorMessage(message + ": The types do not match."));
}

template <typename T>
static inline void checkRange(const T& value, const T& min, const T& max, const std::string& message = "") {
    if (value < min || value > max)
        message.empty() ? throw std::out_of_range(Printers::errorMessage("Value (" + std::to_string(value) + ") is out of the acceptable range. Minimum: " + std::to_string(min) + ", Maximum: " + std::to_string(max) + "."))
                        : throw std::out_of_range(Printers::errorMessage(message + ": Value (" + std::to_string(value) + ") is out of the acceptable range. Minimum: " + std::to_string(min) + ", Maximum: " + std::to_string(max) + "."));
}

static inline void checkIndex(const std::size_t index, const std::size_t size, const std::string& message = "") {
    if (index >= size)
        message.empty() ? throw std::out_of_range(Printers::errorMessage("Index (" + std::to_string(index) + ") is out of bounds. The size is " + std::to_string(size) + "."))
                        : throw std::out_of_range(Printers::errorMessage(message + ": Index (" + std::to_string(index) + ") is out of bounds. The size is " + std::to_string(size) + "."));
}

static inline void checkSquareMatrix(const std::size_t rows, const std::size_t cols, const std::string& message = "") {
    if (rows != cols)
        message.empty() ? throw std::invalid_argument(Printers::errorMessage("Matrix not square. It has " + std::to_string(rows) + " rows and " + std::to_string(cols) + " columns."))
                        : throw std::invalid_argument(Printers::errorMessage(message + ": Matrix not square. It has " + std::to_string(rows) + " rows and " + std::to_string(cols) + " columns."));
}




#endif // CHECKERS_H