#ifndef MATHEMATICAL_EXPRESSIONS_H
#define MATHEMATICAL_EXPRESSIONS_H

#include <cstddef>
#include <type_traits>

namespace Stalker::Mathematics {

template <typename Derived>
class MathematicalExpressionBase {
public:
    template <typename Index> inline auto operator[](Index i) { return _derived()[i]; }
    template <typename Index> inline auto operator[](Index i) const { return _derived()[i]; }
    inline size_t size() const { return _derived()._size(); }

protected:
    inline Derived& _derived() { return static_cast<Derived&>(*this); }
    inline const Derived& _derived() const { return static_cast<const Derived&>(*this); }
};

template <typename Operand>
class Negate : public MathematicalExpressionBase<Negate<Operand>> {
public:
    explicit Negate(const Operand& operand) : _operand(operand) {}
    inline size_t _size() const { return _operand.size(); }
    template <typename Index> inline auto operator[](Index i) const { return -_operand[i]; }

private:
    const Operand& _operand;
};

template <typename Operand, typename Scalar>
class Scale : public MathematicalExpressionBase<Scale<Operand, Scalar>> {
public:

    Scale(const Operand& operand, Scalar scalar) : _operand(operand), _scalar(scalar) {}
    inline size_t _size() const { return _operand.size(); }
    template <typename Index> inline auto operator[](Index i) const { return _operand[i] * _scalar; }

private:
    const Operand& _operand;
    Scalar _scalar;
};

template <typename Operand1, typename Operand2>
class Add : public MathematicalExpressionBase<Add<Operand1, Operand2>> {
public:
    Add(const Operand1& operand1, const Operand2& operand2) : _operand1(operand1), _operand2(operand2) {}
    inline size_t _size() const { return _operand1.size(); }
    template <typename Index> inline auto operator[](Index i) const { return _operand1[i] + _operand2[i]; }

private:
    const Operand1& _operand1;
    const Operand2& _operand2;
};

template <typename Operand1, typename Operand2>
class Subtract : public MathematicalExpressionBase<Subtract<Operand1, Operand2>> {
public:
    Subtract(const Operand1& operand1, const Operand2& operand2) : _operand1(operand1), _operand2(operand2) {}
    inline size_t _size() const { return _operand1.size(); }
    template <typename Index> inline auto operator[](Index i) const { return _operand1[i] - _operand2[i]; }

private:
    const Operand1& _operand1;
    const Operand2& _operand2;
};

template <typename Operand1, typename Operand2>
class Multiply : public MathematicalExpressionBase<Multiply<Operand1, Operand2>> {
public:
    Multiply(const Operand1& operand1, const Operand2& operand2) : _operand1(operand1), _operand2(operand2) {}
    inline size_t _size() const { return _operand1.size(); }
    template <typename Index> inline auto operator[](Index i) const { return _operand1[i] * _operand2[i]; }

private:
    const Operand1& _operand1;
    const Operand2& _operand2;
};

// ================================================================================================================
// ================================================================================================================

template <typename Container>
class ContainerExpressionWrapper {
public:
    explicit ContainerExpressionWrapper(const Container& container) : _container(container) {}
    size_t size() const { return _container.size(); }
    auto operator[](size_t i) const { return _container[i]; }

private:
    const Container& _container;
};

// ================================================================================================================
// ================================================================================================================

template <typename Expression, typename Container>
inline static void evaluate(const Expression& expression, Container& container) {
    for (size_t i = 0; i < container.size(); ++i)
        container[i] = expression[i];
}

template <typename Expression, typename Container>
inline static void evaluate(const Expression& expression, Container& container, size_t size) {
    for (size_t i = 0; i < size; ++i)
        container[i] = expression[i];
}

template <typename Operand>
Negate<Operand> operator-(const MathematicalExpressionBase<Operand>& operand) {
    return Negate<Operand>(static_cast<const Operand&>(operand));
}

// Scalar * Operand
template <typename Operand, typename Scalar,
          typename = std::enable_if_t<std::is_arithmetic_v<Scalar> && !std::is_base_of_v<MathematicalExpressionBase<Operand>, Scalar>>>
auto operator*(Scalar scalar, const Operand& operand) {
    if constexpr (std::is_base_of_v<MathematicalExpressionBase<Operand>, Operand>) {
        // Operand is already an expression
        return Scale<Operand, Scalar>(operand, scalar);
    } else {
        // Operand is a plain container, wrap it
        return Scale<ContainerExpressionWrapper<Operand>, Scalar>(ContainerExpressionWrapper<Operand>(operand), scalar);
    }
}

// Operand * Scalar
template <typename Operand, typename Scalar,
          typename = std::enable_if_t<std::is_arithmetic_v<Scalar> && !std::is_base_of_v<MathematicalExpressionBase<Operand>, Scalar>>>
auto operator*(const Operand& operand, Scalar scalar) {
    if constexpr (std::is_base_of_v<MathematicalExpressionBase<Operand>, Operand>) {
        // Operand is already an expression
        return Scale<Operand, Scalar>(operand, scalar);
    } else {
        // Operand is a plain container, wrap it
        return Scale<ContainerExpressionWrapper<Operand>, Scalar>(ContainerExpressionWrapper<Operand>(operand), scalar);
    }
}

template <typename Operand1, typename Operand2>
Add<Operand1, Operand2> operator+(const MathematicalExpressionBase<Operand1>& lhs, const MathematicalExpressionBase<Operand2>& rhs) {
    return Add<Operand1, Operand2>(static_cast<const Operand1&>(lhs), static_cast<const Operand2&>(rhs));
}

template <typename Operand1, typename Operand2>
Subtract<Operand1, Operand2> operator-(const MathematicalExpressionBase<Operand1>& lhs, const MathematicalExpressionBase<Operand2>& rhs) {
    return Subtract<Operand1, Operand2>(static_cast<const Operand1&>(lhs), static_cast<const Operand2&>(rhs));
}

template <typename Operand1, typename Operand2>
Multiply<Operand1, Operand2> operator*(const MathematicalExpressionBase<Operand1>& lhs, const MathematicalExpressionBase<Operand2>& rhs) {
    return Multiply<Operand1, Operand2>(static_cast<const Operand1&>(lhs), static_cast<const Operand2&>(rhs));
}

} // namespace Stalker::Mathematics
#endif // MATHEMATICAL_EXPRESSIONS_H