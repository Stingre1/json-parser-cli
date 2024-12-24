#pragma once
#include "JsonValue.h"
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
        Context(std::ifstream& file);

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

        
    };

private:
    std::u8string parseKey(Context& context);
    std::unique_ptr<JsonValue> parseValue(Context& context);
    

    std::unique_ptr<JsonValue> parseObject(Context& context);
    std::unique_ptr<JsonValue> parseArray(Context& context);
    std::unique_ptr<JsonValue> parseString(Context& context);
    
    uint32_t parseUnicodeEscape(Context& context);

    static std::u8string encodeUTF8(uint32_t codepoint, int line, int column);
    static uint32_t decodeUTF8(const std::string& input, size_t& pos);

    std::u8string toUtf8(char16_t codepoint); 

    bool isWhitespace(char ch) const;
    void handleLineBreak(char ch);
};

