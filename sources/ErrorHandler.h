#pragma once
#include <iostream>

class ErrorHandler {
public:
    enum class ErrorCode {
        FileNotOpen,
        InvalidEscapeSequence,
        UnexpectedToken,
        TypeMismatch,
        InvalidUnicode,
        UnexpectedEOF,
        TrailingData
    };  

    // Public interface
    static ErrorHandler& getInstance(); // yes there's no reason for a singleton. no i don't care

    void reportError(const std::string& message, size_t line, size_t column);

    void handleError(ErrorCode code, const std::string& details, size_t line, size_t column);

private:
    ErrorHandler() = default;                
    ~ErrorHandler() = default;                
    ErrorHandler(const ErrorHandler&) = delete; // Prevent copying
    ErrorHandler& operator=(const ErrorHandler&) = delete; // Prevent assignment
};
