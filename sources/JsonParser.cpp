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

std::u8string JsonParser::toUtf8(char16_t codepoint) {
    std::u8string u8str;
    //apologies for magic numbers. trust it works.
    if (codepoint <= 0x7F) {
        //is ASCII, can be appended as is
        u8str.push_back(static_cast<char8_t>(codepoint)); 
    } else if (codepoint <= 0x7FF) {
        // is a 2 byte utf8 codepoint
        u8str.push_back(static_cast<char8_t>(0b1100'0000 | (codepoint >> 6)));
        u8str.push_back(static_cast<char8_t>(0b1000'0000 | (codepoint & 0x0011'1111)));
    } else if (codepoint <= 0xFFFF) {
        // is a 4 byte utf8 codepoint
        u8str.push_back(static_cast<char8_t>(0b1110'0000 | (codepoint >> 12)));
        u8str.push_back(static_cast<char8_t>(0b1000'0000 | ((codepoint >> 6) & 0x0011'1111)));
        u8str.push_back(static_cast<char8_t>(0b1000'0000 | (codepoint & 0x0011'1111)));
    }

    return u8str;
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
 
            //if the codepoint is a basic multilingual plane. refer json.org.
            //prepare for magic numbers. (it was unavoidable)
            case 'u' : 
                char hex[4] = {0};
                for(char &it: hex) {
                    file >> it;
                }

                char16_t codeUnit = std::stoi(std::string(hex, 4), nullptr, 16); // converts the string into an integer using base 16

                if (codeUnit < 0xD800 || codeUnit > 0xDFFF) {
                    // is a BMP character, so we encode as UTF-8 and push back
                    str += toUtf8(codeUnit);
                } else if (codeUnit >= 0xD800 && codeUnit <= 0xDBFF) {
                    char next[4];
                    
                    while(file >> ch && std::isspace(ch)); // skip spaces

                    if(ch != '\\') {
                        std::cerr<<"Expected \'\\u\' for surrogate pair.\n";
                    }

                } 
                

            
            //if an invalid escape sequence. simply push back reverse solidus and the character
            default:    
                str.push_back('\\');
                str.push_back(ch);
            }
        } else { // is a regular ascii character and so we push back
            str.push_back(ch);
        }
    }
    return str;
}



