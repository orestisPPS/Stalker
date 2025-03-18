#ifndef TESTUTILITY_H
#define TESTUTILITY_H
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include "../Utility/Printers.h"
#include "../StalkerMathematics/Error.h"

namespace STLKR_Tests {

class TestUtility {

public:

struct DoublePrinterConfig {
    double precision = 6;
    double tolerance = 1e-6;
    bool scientific = false;
};


template <typename T>
static bool compareValues(T computed, T expected, const std::string& message, bool printSuccess = false) {
    bool success = (computed == expected);
    std::string printMessage;
    printSuccess ? printMessage = message + ": Computed: " + std::to_string(computed) + " Expected: " + std::to_string(expected) : printMessage = message;
    printConditionalSuccess(success, printMessage, ColourType::WHITE);
    return success;
}

template <typename T>
static bool compareValues(T computed, T expected, const std::string& message, double tolerance, bool printSuccess = false) {
    bool success = (std::fabs(computed - expected) < tolerance);

    auto printMessage = printSuccess ? message + ": Computed: " + std::to_string(computed) + " Expected: " + std::to_string(expected) : message;
    printConditionalSuccess(success, printMessage, ColourType::WHITE);
    return success;
}

static bool compareDoubles(double computed, double expected, const std::string& message, DoublePrinterConfig config) {
    bool success = (std::fabs(computed - expected) < config.tolerance);

    std::ostringstream computedStream;
    std::ostringstream expectedStream;
    std::ostringstream absErrorStream;

    computedStream << std::fixed << std::setprecision(config.precision);
    expectedStream << std::fixed << std::setprecision(config.precision);
    absErrorStream << std::scientific;

    if (config.scientific) {
        computedStream << std::scientific;
        expectedStream << std::scientific;
    }

    computedStream << computed;
    expectedStream << expected;

    double absError = Error<double>::percentageError(expected, computed);
    absErrorStream << absError;

    std::ostringstream printMessage;
    printMessage << std::setw(10) << std::left << message  
                 << " Computed: " << std::setw(20) << computedStream.str() 
                 << " Expected: " << std::setw(20) << expectedStream.str() 
                 << " AbsErr%:  " << absErrorStream.str();
    
    printConditionalSuccess(success, printMessage.str(), ColourType::WHITE);

    return success;
}


template <typename T>
static bool compareVectors(const T* computed, const T* expected, size_t size, const std::string& message, double tolerance = 1e-9) {
    auto failedIndeces = std::vector<int>();
    for (int i = 0; i < size; ++i) {
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
    printComparisonTablePtr(computed, expected, size, message);

    return failedIndeces.empty();
}

template <typename T>
static bool compareVectors(const std::vector<T>& computed, const std::vector<T>& expected, const std::string& message, double tolerance = 1e-9) {
    return compareVectors(computed.data(), expected.data(), computed.size(), message, tolerance);
}



template <typename T>
static void printComparisonTable(const std::vector<T>& computed, const std::vector<T>& expected,
                                 const std::string& message, bool printSuccess, ColourType colour = ColourType::WHITE, int columnWidth = 10) {
    
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
    static void printComparisonTablePtr(const T* computed, const T* expected, size_t size,  // Changed to const T*
                                        const std::string& message, bool printSuccess = false, 
                                        ColourType colour = ColourType::WHITE, int columnWidth = 10) {
        
        bool testSuccess = true;
        for (size_t i = 0; i < size; ++i) {
            if constexpr (std::is_floating_point<T>::value) {
                testSuccess &= (std::fabs(computed[i] - expected[i]) < 1e-9);
            } else {
                testSuccess &= (computed[i] == expected[i]);
            }
        }

        TestUtility::printConditionalSuccess(testSuccess, message, colour);

        if (!testSuccess || printSuccess) {
            std::cout << std::setw(columnWidth) << "Indices: ";
            for (std::size_t i = 0; i < size; ++i)
                std::cout << std::setw(columnWidth) << i;
            std::cout << std::endl;

            std::cout << std::setw(columnWidth) << "Computed: ";
            for (std::size_t i = 0; i < size; ++i)
                std::cout << std::setw(columnWidth) << computed[i];
            std::cout << std::endl;

            std::cout << std::setw(columnWidth) << "Expected: ";
            for (std::size_t i = 0; i < size; ++i)  // Fixed to use size parameter
                std::cout << std::setw(columnWidth) << expected[i];
            std::cout << std::endl;
            std::cout << std::string(columnWidth * (size + 1), '-') << std::endl;
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

static void printTestTitle(const std::string& title, const std::string& symbol = "=", ColourType colour = ColourType::WHITE) {
    std::cout << std::endl;
    Printers::printTitle(title, symbol, colour);
    std::cout << std::endl;
}



}; // class TestUtility

} // namespace STLKR_Tests

#endif // TESTUTILITY_H