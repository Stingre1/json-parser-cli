#pragma once
#include <iostream>


class ErrorHandler {
public:
    enum class ErrorCode {
        FileNotOpen,
        InvalidEscapeSequence,
        UnexpectedToken,
        InvalidUnicode,
        UnexpectedEOF,
        TrailingData
    };
public:
    // prolly should be private but idk rn
    static void reportError(const std::string& message, size_t line, size_t column);

    static void handleError(ErrorCode code, const std::string& details, 
                            size_t line, size_t column);
};
