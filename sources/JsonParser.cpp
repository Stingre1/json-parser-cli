#include <iostream>
#include <fstream>
#include <cctype>
#include <filesystem>
#include <charconv>
#include <string>
#include "JsonParser.h"
#include "ValueTypes.h"
#include "ErrorHandler.h"

void JsonParser::parse(const std::filesystem::path& path) {

    std::ifstream file(path);
    Context context(file, m_errorHandler);

    context.skipWhitespace();

    char c = context.consume();    
    if (c == '{') {
        parseObject(context);
    } else if (c == '[') {
        parseArray(context);
    } else {
        m_errorHandler.handleError(ErrorCode::UnexpectedToken, std::string(1, c), context.line(), context.column());  
        return;
    }

    // ensures no trailing spaces and then checks for eof
    context.skipWhitespace(); 
    if (!file.eof()) {
        m_errorHandler.handleError(ErrorCode::TrailingData, "", context.line(), context.column());
    } else {
        std::cout << "File parsed successfully.\n";
    }
}

ValueTypes JsonParser::parseObject(Context& context){
    ValueTypes::JsonObject object;
    std::u8string key {};
    
    // we should be inside the object here
    while(true) {
        context.skipWhitespace();
        char c = context.consume();

        switch (c) {
        case '}': { // End of object
             return object; 
        }
        case '"': { // Start of key (which is a string)
            std::u8string key = parseKey(context);

            ValueTypes value = parseValue(context);
            
            object[key] = value; // add key-value pair

            context.skipWhitespace();

            // Check for a trailing comma
            c = context.peek();
            if (c == ',') {
                context.consume(); 
                continue;
            } else if (c == '}') {
                continue; // do nothing (checked anyways in next loop)
            } else {
                m_errorHandler.handleError(ErrorCode::TrailingData, std::string(1, c), context.line(), context.column());
                return object; // not sure if this is the correct approach at this moment. haven't thought how this affects nested objects here
            }
        }
        default:
            m_errorHandler.handleError(ErrorCode::UnexpectedToken, std::string(1, c), context.line(), context.column());
            return object; // not sure if this is the correct approach at this moment. haven't thought how this affects nested objects here
        }
    }
}

std::u8string JsonParser::parseKey(Context& context) {
    // Parse the string for the key
    std::u8string key = parseString(context);
    
    // Ensure the string ends with a closing quote
    char c = context.consume();
    if (c != '"') {
        m_errorHandler.handleError(ErrorCode::UnexpectedToken, std::string(1, c), context.line(), context.column());
    }

    return key;
}

ValueTypes JsonParser::parseValue(Context& context) {
    // Consume leading whitespace
    context.skipWhitespace();

    char c = context.consume();

    // Check the first character to determine the value type
    switch (c) {
    case '"':  // String
        return parseString(context);

    case '{':  // Object
        return parseObject(context);

    case '[':  // Array
        return parseArray(context);

    case 't':  // Boolean (true)
    case 'f':  // Boolean (false)
        return parseBoolean(context, c);

    case 'n':  // Null
        return parseNull(context);

    default:
        if (isdigit(c) || c == '-') {  // Number (integer or decimal)
            return parseNumber(context, c);
        } else {
            m_errorHandler.handleError(ErrorCode::UnexpectedToken, std::string(1, c), context.line(), context.column());
            return {};  // Return an empty ValueType on error
        }
    }
}


ValueTypes JsonParser::parseArray(Context& context){
    
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

ValueTypes JsonParser::parseString(Context& context) {
    unsigned char ch;
    std::u8string str {};
    while (true) {
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

JsonParser::Context::Context(std::ifstream& file, ErrorHandler& errorHandler)
    : m_file(file), m_errorHandler(errorHandler), m_line(1), m_column(0)  {
    if (!m_file.is_open()) {
        m_errorHandler.handleError(ErrorHandler::ErrorCode::FileNotOpen, "", m_line, m_column);
        return;
    }
}

char JsonParser::Context::peek() {
    // Peek the next character in the stream without consuming it
    return static_cast<char>(m_file.peek());
}

char JsonParser::Context::consume() {
    // Consume the next character, updating line and column counters
    char ch = static_cast<char>(m_file.get());
    if (m_file.eof()) {
        m_errorHandler.handleError(ErrorCode::UnexpectedEOF, "", m_line, m_column);
    }

    // Update line and column positions
    if (ch == '\n') {
        ++m_line;
        m_column = 0;
    } else {
        ++m_column;
    }
    return ch;
}


void JsonParser::Context::skipWhitespace() {
    while (m_file && isspace(peek())) {
        consume();
    }
}

size_t JsonParser::Context::line() const { 
    return m_line; 
}

size_t JsonParser::Context::column() const { 
    return m_column; 
}


bool JsonParser::isWhitespace(char ch) const {
    return std::isspace(static_cast<unsigned char>(ch)) != 0;
}

