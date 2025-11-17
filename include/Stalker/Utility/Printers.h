#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>

namespace Stalker::Utility{

/**
 * @enum T_Color
 * @brief Represents ANSI colour codes for terminal text colouring.
 */
enum class T_Color {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_WHITE,
    GRAY,
    ORANGE,
    TOXIC_GREEN,
    DEEP_RED,
    LIGHT_BLUE,
    SHIT_BROWN,
    FERRARI_RED,
    BARBIE_PINK,
    GANDALF_GRAY,
    BUTIAS_ORANGE,
    PATSIOURA_RED,
    RESET
};

/**
 * @brief Retrieves the ANSI escape code for the given T_Color.
 * @param colour The colour to retrieve the code for.
 * @return The ANSI escape code as a string.
 * @throws std::out_of_range If the colour is not defined.
 */
static inline const std::string& getColourCode(T_Color colour) {
    static const std::unordered_map<T_Color, std::string> colourMap = {
        {T_Color::RED,                   "\033[38;5;196m"},  // Punchy Red
        {T_Color::GREEN,                 "\033[38;5;46m"},   // Lush Green
        {T_Color::YELLOW,                "\033[38;5;226m"},  // Bright Yellow
        {T_Color::BLUE,                  "\033[38;5;21m"},   // Vivid Blue
        {T_Color::MAGENTA,               "\033[38;5;201m"},  // Funky Magenta
        {T_Color::CYAN,                  "\033[38;5;51m"},   // Electric Cyan
        {T_Color::WHITE,                 "\033[38;5;15m"},   // Blinding White
        {T_Color::GRAY,                  "\033[38;5;240m"},  // Subtle Gray
        {T_Color::BRIGHT_WHITE,          "\033[1;97m"},      // Bold Bright White
        {T_Color::ORANGE,                "\033[38;5;214m"},  // Zesty Orange
        {T_Color::TOXIC_GREEN,           "\033[38;5;118m"},  // Toxic Green
        {T_Color::DEEP_RED,              "\033[38;5;88m"},   // Deep Red
        {T_Color::LIGHT_BLUE,            "\033[38;5;39m"},   // Light Blue
        {T_Color::SHIT_BROWN,            "\033[38;5;94m"},   // Shit Brown
        {T_Color::FERRARI_RED,           "\033[1;31m"},      // Ferrari Red
        {T_Color::BARBIE_PINK,           "\033[38;5;200m"},  // Barbie Pink
        {T_Color::GANDALF_GRAY,          "\033[38;5;244m"},  // Gandalf Gray
        {T_Color::BUTIAS_ORANGE,         "\033[38;2;254;111;45m"},  // Fotini Patsioura Erwtodikio 1997
        {T_Color::PATSIOURA_RED,   "\033[38;2;253;53;19m"}, // Mr Butias Erwtodikio 1997
        {T_Color::RESET,                 "\033[0m"},         // Reset
    };
    auto it = colourMap.find(colour);
    if (it != colourMap.end()) {
        return it->second;
    } else {
        throw std::out_of_range("Unknown T_Color value");
    }
}

/**
 * @brief Resets the terminal colour to default.
 * @return ANSI escape code string to reset colour.
 */
static inline std::string resetColour() { return "\033[0m"; }

/**
 * @brief Prints a coloured message to the console.
 * @param message The message to print.
 * @param colour The colour to print the message in.
 */
static inline void print(const std::string& message, T_Color colour = T_Color::WHITE) {
    try {
        std::cout << getColourCode(colour) << message << resetColour() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Invalid colour type." << std::endl;
    }
}

/**
 * @brief Generates a formatted string with a title and a message, each with specified colours.
 * 
 * @param title The title to be displayed.
 * @param message The message to be displayed.
 * @param titleColour The colour of the title text. Default is T_Color::WHITE.
 * @param messageColour The colour of the message text. Default is T_Color::WHITE.
 * @return A formatted string with the title and message in specified colours.
 * @throws std::out_of_range if an invalid colour type is provided.
 */
static inline std::string stringWithTitle(const std::string& title, const std::string& message, T_Color titleColour = T_Color::WHITE, T_Color messageColour = T_Color::WHITE) {
    try {
        return getColourCode(titleColour) + title + ": " + resetColour() + getColourCode(messageColour) + message + resetColour();
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Invalid colour type." << std::endl;
        return title + ": " + message; // Return a default formatted string in case of an error
    }
}

/**
 * @brief Prints a message with a title to the standard output.
 *
 * This function prints a message with a specified title, each with their respective colours.
 *
 * @param title The title to be printed.
 * @param message The message to be printed.
 * @param titleColour The colour of the title text. Defaults to T_Color::WHITE.
 * @param messageColour The colour of the message text. Defaults to T_Color::WHITE.
 */
static inline void printWithTitle(const std::string& title, const std::string& message, T_Color titleColour = T_Color::WHITE, T_Color messageColour = T_Color::WHITE) {
    std::cout << stringWithTitle(title, message, titleColour, messageColour) << std::endl;
}

/**
 * @brief Generates an error message string formatted in red.
 * @param message The error message content.
 * @return Formatted error message string with ANSI colour codes.
 */
static inline std::string errorMessage(const std::string& message) { return stringWithTitle("Error", message, T_Color::FERRARI_RED); }

/**
 * @brief Prints an error message in red.
 * @param message The error message to print.
 */
static inline void printError(const std::string& message) { std::cout << errorMessage(message) << std::endl; }

/**
 * @brief Generates a warning message string formatted in orange.
 * @param message The warning message content.
 * @return Formatted warning message string with ANSI colour codes.
 */
static inline std::string warningMessage(const std::string& message) { return stringWithTitle("Warning", message, T_Color::ORANGE); }

/**
 * @brief Prints a warning message in orange.
 * @param message The warning message to print.
 */
static inline void printWarning(const std::string& message) { std::cout << warningMessage(message) << std::endl; }

/**
 * @brief Generates a success message string formatted in green.
 * @param message The success message content.
 * @return Formatted success message string with ANSI colour codes.
 */
static inline std::string successMessage(const std::string& message) { return stringWithTitle("Success", message, T_Color::TOXIC_GREEN); }

/**
 * @brief Prints a success message in green.
 * @param message The success message to print.
 */
static inline void printSuccess(const std::string& message) { std::cout << successMessage(message) << std::endl; }

/**
 * @brief Generates a failure message string formatted in red.
 * @param message The failure message content.
 * @return Formatted failure message string with ANSI colour codes.
 */
static inline std::string failureMessage(const std::string& message) { return stringWithTitle("FAIL", message, T_Color::PATSIOURA_RED, T_Color::WHITE); }

/**
 * @brief Prints a failure message in red.
 * @param message The failure message to print.
 */
static inline void printFailure(const std::string& message) { std::cout << failureMessage(message) << std::endl; }


static inline void printConditionalSuccess(bool condition, const std::string& message) { condition ? printSuccess(message) : printFailure(message); }

/**
 * @brief Generates an informational message string formatted in cyan.
 * @param message The informational message content.
 * @return Formatted informational message string with ANSI colour codes.
 */
static inline std::string infoMessage(const std::string& message) { return stringWithTitle("Info", message, T_Color::GANDALF_GRAY, T_Color::GANDALF_GRAY); }

/**
 * @brief Prints an informational message in cyan.
 * @param message The informational message to print.
 */
static inline void printInfo(const std::string& message) { std::cout << infoMessage(message) << std::endl; }

/**
 * @brief Generates a debug message string formatted in magenta.
 * @param message The debug message content.
 * @return Formatted debug message string with ANSI colour codes.
 */
static inline std::string debugMessage(const std::string& message) { return stringWithTitle("Debug", message, T_Color::MAGENTA); }

/**
 * @brief Prints a debug message in magenta.
 * @param message The debug message to print.
 */
static inline void printDebug(const std::string& message) { std::cout << debugMessage(message) << std::endl; }

static void printTitle(const std::string& title, std::string symbol, T_Color colour = T_Color::WHITE, int padding = 20) {
    if (padding > 50) {
        padding = 50;
    }
    int totalLength = title.length() + 2 * padding;
    std::string border(totalLength, symbol[0]);
    std::string paddedTitle = std::string(padding, ' ') + title + std::string(padding, ' ');

    std::cout << getColourCode(colour) << border << std::endl;
    std::cout << paddedTitle << std::endl;
    std::cout << border << resetColour() << std::endl;
}

static void printSubtitle(const std::string& subtitle, T_Color colour = T_Color::WHITE) {
    std::cout << getColourCode(colour) << "- " << subtitle << resetColour() << std::endl;
}


// ==========================================================================

template <typename T>
static inline std::string arrayToString(const T* array, std::size_t size, int precision = 2) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    oss << "[";
    for (std::size_t i = 0; i < size; ++i) {
        oss << array[i];
        if (i < size - 1) {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

template <typename T>
static inline std::string typeToString() {
    if constexpr (std::is_same_v<T, int>) {
        return "int";
    } else if constexpr (std::is_same_v<T, float>) {
        return "float";
    } else if constexpr (std::is_same_v<T, double>) {
        return "double";
    } else if constexpr (std::is_same_v<T, short>) {
        return "short";
    } else if constexpr (std::is_same_v<T, long>) {
        return "long";
    } else if constexpr (std::is_same_v<T, long long>) {
        return "long long";
    } else if constexpr (std::is_same_v<T, unsigned int>) {
        return "unsigned int";
    } else if constexpr (std::is_same_v<T, unsigned short>) {
        return "unsigned short";
    } else if constexpr (std::is_same_v<T, unsigned long>) {
        return "unsigned long";
    } else if constexpr (std::is_same_v<T, unsigned long long>) {
        return "unsigned long long";
    } else {
        return "Unknown Type";
    }
}

} // namespace Stalker::Utility