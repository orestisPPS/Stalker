#ifndef TESTUTILITY_H
#define TESTUTILITY_H
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include "../Utility/Printers.h"

namespace STLKR_Tests {

class TestUtility {

public:
template <typename T>
static bool compareValues(T computed, T expected, const std::string& message, double tolerance = 1e-9) {
    bool success;
    if constexpr (std::is_floating_point<T>::value) {
        success = std::fabs(computed - expected) < tolerance;
    } else {
        success = computed == expected;
    }
    auto printMessage = message + ": Expected: " + std::to_string(expected) + " Actual: " + std::to_string(computed);
    Printers::printConditionalSuccess(success, printMessage);
    return success;
}

template <typename T>
static bool compareVectors(const std::vector<T>& computed, const std::vector<T>& expected, const std::string& message, double tolerance = 1e-9) {
    Printers::printConditionalSuccess(computed.size() == expected.size(), message + " Vector Size");
    auto failedIndeces = std::vector<int>();
    for (int i = 0; i < computed.size(); ++i) {
        bool success;
        if constexpr (std::is_floating_point<T>::value) {
            success = std::fabs(computed[i] - expected[i]) < tolerance;
        } else {
            success = computed[i] == expected[i];
        }
        if (!success) {
            failedIndeces.push_back(i);
        }
    }
    printComparisonTable(computed, expected, message);
    Printers::printConditionalSuccess(failedIndeces.empty(), message + " Vector Comparison");

    return failedIndeces.empty();
}

template <typename T>
static void printComparisonTable(const std::vector<T>& computed, const std::vector<T>& expected,
                                 const std::string& message, bool printSuccess, ColourType colour = ColourType::GANDALF_GRAY, int columnWidth = 10) {
    
    bool testSuccess = computed == expected;
    TestUtility::printConditionalSuccess(testSuccess, message, colour);

    if (!testSuccess || printSuccess) {
        std::cout << std::setw(columnWidth) << "Indices: ";
        for (std::size_t i = 0; i < computed.size(); ++i)
            std::cout << std::setw(columnWidth) << i;
        std::cout << std::endl;

        std::cout << std::setw(columnWidth) << "Computed: ";
        for (std::size_t i = 0; i < computed.size(); ++i)
            std::cout << std::setw(columnWidth) << computed[i];
        std::cout << std::endl;

        std::cout << std::setw(columnWidth) << "Expected: ";
        for (std::size_t i = 0; i < expected.size(); ++i)
            std::cout << std::setw(columnWidth) << expected[i];
        std::cout << std::endl;
        std::cout << std::string(columnWidth * (computed.size() + 1), '-') << std::endl;
    }
}
template <typename T>
static void printComparisonValues(T computed, T expected, const std::string& message, int columnWidth = 10) {
    std::cout << message << std::endl;

    std::cout << std::setw(columnWidth) << "Computed: " << std::setw(columnWidth) << computed << std::endl;
    std::cout << std::setw(columnWidth) << "Expected: " << std::setw(columnWidth) << expected << std::endl;

    std::cout << std::string(columnWidth * 2, '-') << std::endl;
}

static void printPassed(const std::string& message, ColourType colour = ColourType::WHITE) {
    Printers::printWithTitle("[PASS]", message, ColourType::GREEN, colour);
}

static void printFail(const std::string& message, ColourType colour = ColourType::WHITE) {
    Printers::printWithTitle("[FAIL]", message, ColourType::PATSIOURA_RED, colour);
}

static void printConditionalSuccess(bool condition, const std::string& message, ColourType colour = ColourType::WHITE) {
    condition ? printPassed(message, colour) : printFail(message, colour);
}

}; // class TestUtility

} // namespace STLKR_Tests

#endif // TESTUTILITY_H