#pragma once
#include "ValueType.h"
#include <stack>
#include <fstream>

class JsonParser {
private:
    size_t lineNumber;
public:
    JsonParser();
    
    void parse(const std::ifstream& file);
};

