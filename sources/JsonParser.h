#pragma once
#include "ValueTypes.h"
#include "ErrorHandler.h"
#include <stack>
#include <fstream>
#include <filesystem>

class JsonParser {
public:
    JsonParser();
    void parse(const std::filesystem::path& path);

private:
    ErrorHandler m_errorHandler;
    using ErrorCode = ErrorHandler::ErrorCode;

    class Context {
    public:
        Context(std::ifstream& file, ErrorHandler& rrorHandler);

        char peek();
        char consume();
        void skipWhitespace();
    public:
        size_t line() const { return m_line; }
        size_t column() const { return m_column; }
    private:
        std::ifstream& m_file;
        size_t m_line;
        size_t m_column;

        //have to do this so consume() can use m_errorHandler
        ErrorHandler& m_errorHandler;
    };

private:
    std::u8string parseKey(Context& context);
    ValueTypes parseValue(Context& context);
    

    ValueTypes parseObject(Context& context);
    ValueTypes parseArray(Context& context);
    std::u8string parseString(Context& context);
    
    uint32_t parseUnicodeEscape(Context& context);

    static std::u8string encodeUTF8(uint32_t codepoint);
    static uint32_t decodeUTF8(const std::string& input, size_t& pos);

    std::u8string toUtf8(char16_t codepoint); 

    bool isWhitespace(char ch) const;
    void handleLineBreak(char ch);
};

