#include "JsonParser.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <filesystem>
#include "ValueType.h"
#include <string>

JsonParser::JsonParser() : lineNumber(0) {}

bool JsonParser::checkNewLineCharacter(char ch) {
    return ch == '\n' ? true : false;
}

void JsonParser::checkAndIncrementLineCount(char ch){
    if(checkNewLineCharacter(ch)) {
        lineNumber++;
    }
}

void JsonParser::parse(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: File is not open." << std::endl;
        return;
    }

    char c;    
    while(file >> c) {
        checkAndIncrementLineCount(c);
        if(std::isspace(static_cast<unsigned char>(c))) { continue; }
        switch(c) {
        case '{':
            parseObject(file);
            break;
        case '[':
           parseArray(file);
        default:
            std::cerr<<"Error parsing array at line "<< lineNumber << std::endl;
        }
    }
    if(file.eof()) {
        std::cout<<"File parsed successfully.\n";
        return;
    } else {
        std::cout<<"Error parsing file.\n";
    }
}

ValueType JsonParser::parseObject(std::ifstream& file){
    char c;
    ValueType::JSONObject object;
    std::u8string key {};

    while(file >> c) {
        checkAndIncrementLineCount(c);
        if(std::isspace(c)) { continue; }

        if(c == '}') { return true; }

        switch(c) {
        //check for key
        case '"':
            key = parseString(file);
            break;
        case ':' :
            break;
        }

        

    }
}

ValueType JsonParser::parseArray(std::ifstream& file){
    
}

std::u8string JsonParser::parseString(std::ifstream& file) {
    unsigned char ch;
    std::u8string str {};
    while (file >> ch) {
        if(std::isspace(ch)) { continue; }
        if(ch == '"') { return str; }

        //any byte greater than 127 (0x7F), is not in ASCII so it can't simply pushed back
        if(ch > 0x7F) {
            //handle unicode
            continue;
        }

        //parse escape sequences
        if(ch == '\\') {
            file >> ch;
            switch(ch) {
            case '"' : str.push_back('\"'); break;
            case '\\' : str.push_back('\\'); break;
            case '/' : str.push_back('/'); break;
            case 'b' : str.push_back('\b'); break;
            case 'f' : str.push_back('\f'); break;
            case 'n' : str.push_back('\n'); break;
            case 'r' : str.push_back('\r'); break;
            case 't' : str.push_back('\t'); break;

            //codepoint is a basic multilingual plane. refer json.org.
            //prepare for magic numbers. (it was unavoidable)
            case 'u' : 
                char16_t bmp {0};
                

            
            //if an invalid escape sequence. simply push back reverse solidus and the character
            default:    
                str.push_back('\\');
                str.push_back(ch);
            }
        }
    }
    return str;
}

