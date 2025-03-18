#ifndef FINITE_DIFFERENCE_SCHEME_H
#define FINITE_DIFFERENCE_SCHEME_H

#include <array>

enum class SchemeType {
        Forward,
        Backward,
        Central,
        Arbitrary,
        None
};

enum class ErrorOrder {
        O_1,
        O_2,
        O_3,
        O_4,
        O_5,
        O_6,
        None
};

using DerivativeOrder = unsigned int;

template <DerivativeOrder Order, SchemeType Scheme, ErrorOrder Error>
struct FiniteDifferenceScheme;

//=================================================================================================
//=================================================================================================
//                                      First Derivative 
//                      Finite Difference Schemes Template Specializations
//=================================================================================================
//=================================================================================================


//=================================================================================================
//                              Scheme Type : Central Error Order : 2
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Central, ErrorOrder::O_2> {

    static constexpr inline size_t                nPoints()             { return 3; }
    static constexpr inline std::array<double, 3> weights()             { return { -1,  0,  1 }; }
    static constexpr inline std::array<int, 3>    stencil()             { return { -1,  0,  1 }; }
    static constexpr inline double                denominator(double h) { return 2 * h; }
};


//=================================================================================================
//                              Scheme Type : Central Error Order : 4
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Central, ErrorOrder::O_4> {

    static constexpr inline size_t                nPoints()             { return 5; }
    static constexpr inline std::array<double, 5> weights()             { return {  1, -8,  0,  8, -1 }; }
    static constexpr inline std::array<int, 5>    stencil()             { return { -2, -1,  0,  1,  2 }; }
    static constexpr inline double                denominator(double h) { return 12 * h; }
};

//=================================================================================================
//                              Scheme Type : Central Error Order : 6
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Central, ErrorOrder::O_6> {

    static constexpr inline size_t                nPoints()             { return 7; }
    static constexpr inline std::array<double, 7> weights()             { return {  -1,   9, -45,   0,  45,  -9,   1 }; }
    static constexpr inline std::array<int, 7>    stencil()             { return {  -3,  -2,  -1,   0,   1,   2,   3 }; }
    static constexpr inline double                denominator(double h) { return 60 * h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 1
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Forward, ErrorOrder::O_1> {

    static constexpr inline size_t                nPoints()             { return 2; }
    static constexpr inline std::array<double, 2> weights()             { return { -1,  1 }; }
    static constexpr inline std::array<int, 2>    stencil()             { return {  0,  1 }; }
    static constexpr inline double                denominator(double h) { return h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 2
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Forward, ErrorOrder::O_2> {

    static constexpr inline size_t                nPoints()             { return 3; }
    static constexpr inline std::array<double, 3> weights()             { return { -3,  4, -1 }; }
    static constexpr inline std::array<int, 3>    stencil()             { return {  0,  1,  2 }; }
    static constexpr inline double                denominator(double h) { return 2 * h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 3
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Forward, ErrorOrder::O_3> {

    static constexpr inline size_t                nPoints()             { return 4; }
    static constexpr inline std::array<double, 4> weights()             { return {-11, 18, -9,  2 }; }
    static constexpr inline std::array<int, 4>    stencil()             { return {  0,  1,  2,  3 }; }
    static constexpr inline double                denominator(double h) { return 6 * h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 4
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Forward, ErrorOrder::O_4> {

    static constexpr inline size_t                nPoints()             { return 5; }
    static constexpr inline std::array<double, 5> weights()             { return {-25, 48, -36, 16, -3 }; }
    static constexpr inline std::array<int, 5>    stencil()             { return {  0,  1,   2,  3,  4 }; }
    static constexpr inline double                denominator(double h) { return 12 * h; }
};

//=================================================================================================
//                               Scheme Type : Forward Error Order : 5
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Forward, ErrorOrder::O_5> {

    static constexpr inline size_t                nPoints()             { return 6; }
    static constexpr inline std::array<double, 6> weights()             { return {-137, 300, -300, 200, -75, 12 }; }
    static constexpr inline std::array<int, 6>    stencil()             { return {   0,   1,    2,   3,   4,  5 }; }
    static constexpr inline double                denominator(double h) { return 60 * h; }
};


//=================================================================================================
//                              Scheme Type : Backward Error Order : 1
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Backward, ErrorOrder::O_1> {

    static constexpr inline size_t                nPoints()             { return 2; }
    static constexpr inline std::array<double, 2> weights()             { return { -1,  1 }; }
    static constexpr inline std::array<int, 2>    stencil()             { return { -1,  0 }; }
    static constexpr inline double                denominator(double h) { return h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 2
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Backward, ErrorOrder::O_2> {

    static constexpr inline size_t                nPoints()             { return 3; }
    static constexpr inline std::array<double, 3> weights()             { return {  1, -4,  3 }; }
    static constexpr inline std::array<int, 3>    stencil()             { return { -2, -1,  0 }; }
    static constexpr inline double                denominator(double h) { return 2 * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 3
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Backward, ErrorOrder::O_3> {

    static constexpr inline size_t                nPoints()             { return 4; }
    static constexpr inline std::array<double, 4> weights()             { return { -2,  9, -18, 11 }; }
    static constexpr inline std::array<int, 4>    stencil()             { return { -3, -2,  -1,  0 }; }
    static constexpr inline double                denominator(double h) { return 6 * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 4
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Backward, ErrorOrder::O_4> {

    static constexpr inline size_t                nPoints()             { return 5; }
    static constexpr inline std::array<double, 5> weights()             { return {  3, -16, 36, -48, 25 }; }
    static constexpr inline std::array<int, 5>    stencil()             { return { -4,  -3, -2,  -1,  0 }; }
    static constexpr inline double                denominator(double h) { return 12 * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 5
//=================================================================================================
template <>
struct FiniteDifferenceScheme<1, SchemeType::Backward, ErrorOrder::O_5> {

    static constexpr inline size_t                nPoints()             { return 6; }//-137, 300, -300, 200, -75, 12
    static constexpr inline std::array<double, 6> weights()             { return { -12, 75, -200, 300, -300, 137 }; }
    static constexpr inline std::array<int, 6>    stencil()             { return {  -5, -4,   -3,  -2,  -1,  0 }; }
    static constexpr inline double                denominator(double h) { return 60 * h; }
};

//=================================================================================================
//=================================================================================================
//                                      Second Derivative 
//                      Finite Difference Schemes Template Specializations
//=================================================================================================
//=================================================================================================

//=================================================================================================
//                              Scheme Type : Central Error Order : 2
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Central, ErrorOrder::O_2> {

    static constexpr inline size_t                nPoints()             { return 3; }
    static constexpr inline std::array<double, 3> weights()             { return {1, -2, 1}; }
    static constexpr inline std::array<int, 3>    stencil()             { return {-1, 0, 1}; }
    static constexpr inline double                denominator(double h) { return h * h; }
};

//=================================================================================================
//                              Scheme Type : Central Error Order : 4
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Central, ErrorOrder::O_4> {

    static constexpr inline size_t                nPoints()             { return 5; }
    static constexpr inline std::array<double, 5> weights()             { return {-1, 16, -30, 16, -1}; }
    static constexpr inline std::array<int, 5>    stencil()             { return {-2, -1, 0, 1, 2}; }
    static constexpr inline double                denominator(double h) { return 12 * h * h; }
};

//=================================================================================================
//                              Scheme Type : Central Error Order : 6
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Central, ErrorOrder::O_6> {

    static constexpr inline size_t                nPoints()             { return 7; }
    static constexpr inline std::array<double, 7> weights()             { return {2, -27, 270, -490, 270, -27, 2}; }
    static constexpr inline std::array<int, 7>    stencil()             { return {-3, -2, -1, 0, 1, 2, 3}; }
    static constexpr inline double                denominator(double h) { return 180 * h * h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 2
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Forward, ErrorOrder::O_2> {

    static constexpr inline size_t                nPoints()             { return 4; }
    static constexpr inline std::array<double, 4> weights()             { return { 2, -5, 4, -1}; }
    static constexpr inline std::array<int, 4>    stencil()             { return { 0,  1, 2, 3}; }
    static constexpr inline double                denominator(double h) { return h * h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 3
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Forward, ErrorOrder::O_3> {

    static constexpr inline size_t                nPoints()             { return 5; }
    static constexpr inline std::array<double, 5> weights()             { return { 35, -104, 114, -56, 11}; }
    static constexpr inline std::array<int, 5>    stencil()             { return { 0,   1,   2,  3, 4}; }
    static constexpr inline double                denominator(double h) { return 12 * h * h; }
};

//=================================================================================================
//                              Scheme Type : Forward Error Order : 4
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Forward, ErrorOrder::O_4> {

    static constexpr inline size_t                nPoints()             { return 6; }
    static constexpr inline std::array<double, 6> weights()             { return { 45, -154, 214, -156, 61, -10}; }
    static constexpr inline std::array<int, 6>    stencil()             { return {  0,    1,   2,    3,  4,   5}; }
    static constexpr inline double                denominator(double h) { return 12 * h * h; }
};


//=================================================================================================
//                              Scheme Type : Forward Error Order : 5
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Forward, ErrorOrder::O_5> {

    static constexpr inline size_t                nPoints()             { return 7; }
    static constexpr inline std::array<double, 7> weights()             { return { 812, -3132, 5265, -5080, 2970, -972, 137}; }
    static constexpr inline std::array<int, 7>    stencil()             { return {  0,    1,    2,    3,   4,   5,  6}; }
    static constexpr inline double                denominator(double h) { return 180 * h * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 2
//=================================================================================================

template <>
struct FiniteDifferenceScheme<2, SchemeType::Backward, ErrorOrder::O_2> {

    static constexpr inline size_t                nPoints()             { return 4; }
    static constexpr inline std::array<double, 4> weights()             { return { -1, 4, -5, 2}; }
    static constexpr inline std::array<int, 4>    stencil()             { return {-3, -2, -1, 0}; }
    static constexpr inline double                denominator(double h) { return h * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 3
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Backward, ErrorOrder::O_3> {

    static constexpr inline size_t                nPoints()             { return 5; }
    static constexpr inline std::array<double, 5> weights()             { return { 11, -56, 114, -104, 35}; }
    static constexpr inline std::array<int, 5>    stencil()             { return {-4, -3, -2, -1}; }
    static constexpr inline double                denominator(double h) { return 12 * h * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 4
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Backward, ErrorOrder::O_4> {

    static constexpr inline size_t                nPoints()             { return 6; }
    static constexpr inline std::array<double, 6> weights()             { return { -10, 61, -156, 214, -154, 45}; }
    static constexpr inline std::array<int, 6>    stencil()             { return {-5, -4, -3, -2, -1,  0}; }
    static constexpr inline double                denominator(double h) { return 12 * h * h; }
};

//=================================================================================================
//                              Scheme Type : Backward Error Order : 5
//=================================================================================================
template <>
struct FiniteDifferenceScheme<2, SchemeType::Backward, ErrorOrder::O_5> {

    static constexpr inline size_t                nPoints()             { return 7; }
    static constexpr inline std::array<double, 7> weights()             { return { 137, -972, 2970, -5080, 5265, -3132, 812}; }
    static constexpr inline std::array<int, 7>    stencil()             { return {-6, -5, -4, -3, -2, -1,  0}; }
    static constexpr inline double                denominator(double h) { return 180 * h * h; }
};

#endif // FINITE_DIFFERENCE_SCHEME_H