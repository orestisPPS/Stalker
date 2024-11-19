//
// Created by hal9000 on 9/2/24.
//

#ifndef STALKER_STALKERARRAY_H
#define STALKER_STALKERARRAY_H

#include <array>
#include <stdexcept>
#include "MetaMathOperations.h"

/**
 * \brief A fixed-size array container.
 * 
 * \tparam size The size of the array.
 * \tparam T The type of elements stored in the array.
 */
template<unsigned size, typename T>
class StalkerArray {
public:
    /**
     * \brief Default constructor.
     */
    StalkerArray() = default;

    /**
     * \brief Constructs the array from a std::array.
     * 
     * \param data The std::array to copy data from.
     */
    explicit StalkerArray(const std::array<T, size> &data) : _data(data) { }

    /**
     * \brief Constructs the array from a raw array.
     * 
     * \param data The raw array to copy data from.
     */
    explicit StalkerArray(const T *data) {
        for (unsigned i = 0; i < size; i++)
            _data[i] = data[i];
    }

    /**
     * \brief Constructs the array with a default value.
     * 
     * \param defaultValue The value to initialize all elements with.
     */
    explicit StalkerArray(const T &defaultValue) { _data.fill(defaultValue); }

    /**
     * \brief Constructs the array from an initializer list.
     * 
     * \param initList The initializer list to copy data from.
     * \throws std::invalid_argument if the initializer list size does not match the array size.
     */
    StalkerArray(std::initializer_list<T> initList) {
        if (initList.size() != size)
            throw std::invalid_argument("Initializer list size does not match array size");
        std::copy(initList.begin(), initList.end(), _data.begin());
    }

    /**
     * \brief Copy constructor.
     * 
     * \param other The other StalkerArray to copy data from.
     */
    StalkerArray(const StalkerArray &other) : _data(other._data) { }

    /**
     * \brief Move constructor.
     * 
     * \param other The other StalkerArray to move data from.
     */
    StalkerArray(StalkerArray &&other) noexcept : _data(std::move(other._data)) { }

    /**
    * \brief Copy assignment operator.
    * 
    * \param other The other StalkerArray to copy data from.
    * \return A reference to this StalkerArray.
    */
    StalkerArray &operator=(const StalkerArray &other) {
        if (this != &other)
            _data = other._data;
        return *this;
    }

    /**
    * \brief Move assignment operator.
    * 
    * \param other The other StalkerArray to move data from.
    * \return A reference to this StalkerArray.
    */
    StalkerArray &operator=(StalkerArray &&other) noexcept {
        if (this != &other)
            _data = std::move(other._data);
        return *this;
    }

    /**
    * \brief Adds two arrays element-wise.
    *
    * \param other The other array to add.
    * \return A new StalkerArray with the element-wise sum.
    */
    StalkerArray<size, T> operator+(const StalkerArray<size, T> &other) const {
        StalkerArray<size, T> result;
        add(other, result);
        return result;
    }

    /**
    * \brief Subtracts another array from this array element-wise.
    *
    * \param other The other array to subtract.
    * \return A new StalkerArray with the element-wise difference.
    */
    StalkerArray<size, T> operator-(const StalkerArray<size, T> &other) const {
        StalkerArray<size, T> result;
        subtract(other, result);
        return result;
    }

    /**
    * \brief Multiplies two arrays element-wise.
    *
    * \param other The other array to multiply.
    * \return A new StalkerArray with the element-wise product.
    */
    StalkerArray<size, T> operator*(const StalkerArray<size, T> &other) const {
        StalkerArray<size, T> result;
        multiply(other, result);
        return result;
    }

    /**
     * \brief Accesses the element at the given index.
     * 
     * \param index The index of the element to access.
     * \return A reference to the element at the given index.
     */
    T &operator[](unsigned index) { return _data[index]; }

    /**
     * \brief Accesses the element at the given index.
     * 
     * \param index The index of the element to access.
     * \return A const reference to the element at the given index.
     */
    const T &operator[](unsigned index) const { return _data[index]; }

    /**
     * \brief Gets the size of the array.
     * 
     * \return The size of the array.
     */
    [[nodiscard]] unsigned getSize() const { return size; }

    /**
     * \brief Gets the underlying std::array.
     * 
     * \return A const reference to the underlying std::array.
     */
    [[nodiscard]] const std::array<T, size> &getData() const { return _data; }

    /**
     * \brief Sets the underlying std::array.
     * 
     * \param data The std::array to set.
     */
    void setData(const std::array<T, size> &data) { _data = data; }

    /**
     * \brief Sets the underlying std::array.
     * 
     * \param data The std::array to set.
     */
    void setData(std::array<T, size> &&data) { _data = std::move(data); }

    /**
     * \brief Fills the array with a value.
     * 
     * \param value The value to fill the array with.
     */
    void fill(const T &value) { _data.fill(value); }
    
    /**
    * \brief Computes the power of each element in the array.
    *
    * \tparam Power The exponent to raise each element to.
    * \return A new StalkerArray with each element raised to the given power.
    */
    template<unsigned Power>
    StalkerArray<size, T> power() const {
    StalkerArray<size, T> result;
    for (unsigned i = 0; i < size; i++)
        result[i] = MetaMathOperations::power<T, Power>(_data[i]);
    return result;
    }
    
    /**
    * \brief Computes the sum of the powers of the elements in the array.
    *
    * \tparam Power The exponent to raise each element to before summing.
    * \return The sum of each element raised to the given power.
    */
    template<unsigned Power = 1>
    constexpr T sumOfPower() const {
        return MetaMathOperations::sumOfPower<T, size, Power>(_data.data());
    }

    /**
    * \brief Computes the sum of the elements in the array.
    *
    * \return The sum of each element raised to the given power.
    */
    constexpr T sum() const {
        return MetaMathOperations::sumOfPower<T, size, 1>(_data.data());
    }
    /**
    * \brief Adds two arrays element-wise with optional scalar multipliers.
    *
    * \param other The other array to add.
    * \param result The array to store the result.
    * \param scalar1 The scalar multiplier of this array.
    * \param scalar2 The scalar multiplier of the other array.
    */
    void add(const StalkerArray<size, T> &other, StalkerArray<size, T> &result, T scalar1 = 1, T scalar2 = 1) const {
        MetaMathOperations::add<T, size>(_data.data(), other._data.data(), result._data.data(), scalar1, scalar2);
    }
    
    /**
    * \brief Subtracts another array from this array element-wise with optional scalar multipliers.
    *
    * \param other The other array to subtract.
    * \param result The array to store the result.
    * \param scalar1 The scalar multiplier of this array.
    * \param scalar2 The scalar multiplier of the other array.
    */
    void subtract(const StalkerArray<size, T> &other, StalkerArray<size, T> &result, T scalar1 = 1, T scalar2 = 1) const {
        MetaMathOperations::subtract<T, size>(_data.data(), other._data.data(), result._data.data(), scalar1, scalar2);
    }

    /**
    * \brief Multiplies two arrays element-wise with optional scalar multipliers.
    *
    * \param other The other array to multiply.
    * \param result The array to store the result.
    * \param scalar1 The scalar multiplier of this array.
    * \param scalar2 The scalar multiplier of the other array.
    */
    void multiply(const StalkerArray<size, T> &other, StalkerArray<size, T> &result, T scalar1 = 1, T scalar2 = 1) const {
        MetaMathOperations::multiply<T, size>(_data.data(), other._data.data(), result._data.data(), scalar1, scalar2);
    }
    
    /**
    * \brief Computes the dot product of the elements in the array.
    *
    * \return The dot product of the elements in the array.
    */
    T dotProduct() const {
        return MetaMathOperations::dotProduct<T, size>(_data.data());
    }
    
    /**
    * \brief Computes the factorial of the size of the array.
    *
    * \return The factorial of the size of the array.
    */
    T factorial() const {
        return MetaMathOperations::factorial<T, size>();
    }
    
    /**
    * \brief Computes the Fibonacci number at the position of the size of the array.
    *
    * \return The Fibonacci number at the position of the size of the array.
    */
    T fibonacci() const {
        return MetaMathOperations::fibonacci<T, size>();
    }

private:
    std::array<T, size> _data; ///< The underlying std::array.
    
    
};

struct ValidVectorPart{
    unsigned start;
    unsigned end;
};
#endif //STALKER_STALKERARRAY_H