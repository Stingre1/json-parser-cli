#pragma once
#include "ValueType.h"
#include <stack>
#include <fstream>
#include <filesystem>

class JsonParser {
private:
    size_t lineNumber;
public:
    JsonParser();
    
    void parse(const std::filesystem::path& path);

private:
    ValueType parseObject(std::ifstream& file);
    ValueType parseArray(std::ifstream& file);
    std::u8string parseString(std::ifstream& file);
    ValueType parseValue(std::ifstream& file);
    bool checkNewLineCharacter(char ch);
    void checkAndIncrementLineCount(char ch);
};

