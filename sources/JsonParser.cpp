#include "JsonParser.h"
#include <iostream>
#include <fstream>
#include <cctype>

JsonParser::JsonParser() {
    lineNumber = 0;
}

void JsonParser::parse(const std::ifstream& file) {
    if (!file.is_open()) {
        std::cerr << "Error: File is not open." << std::endl;
        return;
    }

    // unsigned char ch;
    // while (file.get(ch)) {  f
        
    //     if (!std::isspace(ch)) {
    //         std::cout << "Found non-whitespace character: " << ch << std::endl;
    //         break;
    //     }
    // }
    // if (file.eof()) {
    //     std::cout << "Error parsing file. No valid object or array found." << std::endl;
    // }

}

