#pragma once
#include "ValueType.h"
#include <stack>
#include <fstream>
#include <filesystem>

//TODO: change errors to be errorCodes and not std::cerr
class JsonParser {
private:
    size_t lineNumber;
    enum class ErrorCode {
        
    };
public:
    JsonParser();
    void reportError(ErrorCode errCode, const std::string& context);
    void parse(const std::filesystem::path& path);
    ValueType parseObject(std::ifstream& file);
    ValueType parseArray(std::ifstream& file);
    std::u8string parseString(std::ifstream& file);
    ValueType parseValue(std::ifstream& file);

private:
    std::u8string toUtf8(char16_t codepoint); // encodes a valid BMP character into utf8

    inline bool checkNewLineCharacter(char ch);
    inline void checkAndIncrementLineCount(char ch);
};

