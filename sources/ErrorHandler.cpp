#include "ErrorHandler.h"

void ErrorHandler::reportError(const std::string& message, size_t line, size_t column) {
    std::cerr << "Error: " << message 
              << " at line " << line 
              << ", column " << column << "." << std::endl;
}

void ErrorHandler::handleError(ErrorCode code, const std::string& details, size_t line, size_t column) {
    std::string errorMessage;

    switch (code) {
        case ErrorCode::FileNotOpen:
            errorMessage = "Failed to open file";
            break;
        case ErrorCode::InvalidEscapeSequence:
            errorMessage = "Invalid escape sequence";
            break;
        case ErrorCode::UnexpectedToken:
            errorMessage = "Unexpected token: " + details;
            break;
        case ErrorCode::InvalidUnicode:
            errorMessage = "Invalid Unicode sequence";
            break;
        case ErrorCode::TrailingData:
            errorMessage = "Trailing data after JSON value";
            break;
        case ErrorCode::UnexpectedEOF:
            errorMessage = "Unexpected End of File";
            break;
        default:
            errorMessage = "Unknown error";
            break;
    }

    reportError(errorMessage, line, column);
}

