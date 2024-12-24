#include <iostream>
#include <fstream>
#include <cctype>
#include <filesystem>
#include <charconv>
#include <string>
#include "JsonParser.h"
#include "JsonValue.h"
#include "ErrorHandler.h"

void JsonParser::parse(const std::filesystem::path& path) {

    std::ifstream file(path);
    Context context(file);

    context.skipWhitespace();
    char c = context.consume();    
    if (c == '{') {
        parseObject(context);
    } else if (c == '[') {
        parseArray(context);
    } else {
        ErrorHandler::getInstance().handleError(
            ErrorCode::UnexpectedToken,
            std::string(1, c), 
            context.line(), 
            context.column());  
        return;
    }

    // ensures no trailing spaces and then checks for eof
    context.skipWhitespace(); 
    if (!file.eof()) {
        ErrorHandler::getInstance().handleError(
            ErrorCode::TrailingData, 
            "",
            context.line(),
            context.column());
    } else {
        std::cout << "File parsed successfully.\n";
    }
}

std::unique_ptr<JsonValue> JsonParser::parseObject(Context& context){
    auto object = std::make_unique<JsonValue::JsonObject>();
    std::u8string key {};
    
    // we should be inside the object here
    while(true) {
        context.skipWhitespace();
        char c = context.consume();

        switch (c) {
        case '}': { // End of object
            return std::make_unique<JsonValue>(std::move(*object)); 
        }
        case '"': { // Start of key (which is a string)
            std::u8string key = parseKey(context);

            auto value = parseValue(context);
            
            object->emplace(std::move(key), std::make_unique<JsonValue>(std::move(value))); 

            context.skipWhitespace();

            // Check for a trailing comma
            c = context.peek();
            if (c == ',') {
                context.consume(); 
                continue;
            } else if (c == '}') {
                continue; // do nothing (checked anyways in next loop)
            } else {
                ErrorHandler::getInstance().handleError(
                    ErrorCode::TrailingData, 
                    std::string(1, c), 
                    context.line(), 
                    context.column());
                return std::make_unique<JsonValue>(std::move(*object));  // not sure if this is the correct approach at this moment. haven't thought how this affects nested objects here
            }
        }
        default:
            ErrorHandler::getInstance().handleError(
                ErrorCode::UnexpectedToken, 
                std::string(1, c), 
                context.line(), 
                context.column());
            return std::make_unique<JsonValue>(std::move(*object));  // not sure if this is the correct approach at this moment. haven't thought how this affects nested objects here
        }
    }
}

std::u8string JsonParser::parseKey(Context& context) {
    // Parse the string for the key
    auto key = parseString(context);
    
    // Ensure the string ends with a closing quote
    char c = context.consume();
    if (c != '"') {
        ErrorHandler::getInstance().handleError(
            ErrorCode::UnexpectedToken, 
            std::string(1, c), 
            context.line(), 
            context.column()
        );
    }

    return key->getString();
}

std::unique_ptr<JsonValue> JsonParser::parseValue(Context& context) {
    // Consume leading whitespace
    context.skipWhitespace();

    char c = context.consume();

    // Check the first character to determine the value type
    switch (c) {
    case '"':  // String
        return parseString(context);

    case '{':  // Object
        // return parseObject(context);

    case '[':  // Array
        return parseArray(context);

    case 't':  // Boolean (true)
    case 'f':  // Boolean (false)
        // return parseBoolean(context, c);

    case 'n':  // Null
        // return parseNull(context);

    default:
        // if (isdigit(c) || c == '-') {  // Number (integer or decimal)
        //     return parseNumber(context, c);
        // } else {
        //     ErrorHandler::getInstance().handleError(ErrorCode::UnexpectedToken, std::string(1, c), context.line(), context.column());
        //     return {};
        // }
    }
}

std::unique_ptr<JsonValue> JsonParser::parseString(Context& context) {
    std::u8string str {};
    while (true) {
        char ch = context.consume();
        if(ch == '"') {
            return std::make_unique<JsonValue>(std::move(str)); 
        }

        //any byte greater than 127 (0x7F), is not in ASCII so it can't simply pushed back
        if(ch > 0x7F) {
            //handle unicode
            continue;
        }

        //parse escape sequences
        if(ch == '\\') {
            switch(ch) {
            case '"' : str.push_back('\"'); break;
            case '\\' : str.push_back('\\'); break;
            case '/' : str.push_back('/'); break;
            case 'b' : str.push_back('\b'); break;
            case 'f' : str.push_back('\f'); break;
            case 'n' : str.push_back('\n'); break;
            case 'r' : str.push_back('\r'); break;
            case 't' : str.push_back('\t'); break;
 
            // if the codepoint is a basic multilingual plane (BMP). see: json.org.
            case 'u' : {
                uint32_t codepoint = parseUnicodeEscape(context);
                str += encodeUTF8(codepoint, context.line(), context.column());
                break;
            }
            // if an invalid escape sequence. simply push back reverse solidus and the character
            default:
                ErrorHandler::getInstance().handleError(
                        ErrorCode::InvalidEscapeSequence,
                        std::string(1, ch),
                        context.line(),
                        context.column()
                    );
                str.push_back(u8'\\');
                str.push_back(static_cast<char8_t>(ch));    
            }
        } else { // is a regular ascii character and so we push back
            str.push_back(ch);
        }
    }

    return std::make_unique<JsonValue>(std::move(str));
}

uint32_t JsonParser::parseUnicodeEscape(Context& context) {
    // Parse the first 4 hex digits into a codepoint
    std::string hexStr;
    hexStr.reserve(4);
    
    for (int i = 0; i < 4; ++i) {
        char ch = context.consume();
        if (!std::isxdigit(ch)) { // check if ch is a hex digit
            ErrorHandler::getInstance().handleError(
                ErrorCode::InvalidUnicode,
                std::string(1, ch),
                context.line(),
                context.column()
            );
            return 0;
        }
        hexStr.push_back(ch);
    }
    
    uint32_t codepoint = std::stoul(hexStr, nullptr, 16); // convert str to unsigned long
    
    // Check for high surrogate (0xD800-0xDBFF)
    // 0xD800 = 1101'1000 0000'0000
    // 0xDBFF = 1101'1011 1111'1111
    if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
        // This is a high surrogate, expect a low surrogate
        char next1 = context.consume();
        char next2 = context.consume();
        
        // check for '\u'
        if (next1 != '\\' || next2 != 'u') {
            ErrorHandler::getInstance().handleError(
                ErrorCode::InvalidUnicode,
                "Expected low surrogate",
                context.line(),
                context.column()
            );
            // Return the high surrogate codepoint if the low surrogate is invalid
            return codepoint; 
        }
        
        hexStr.clear();
        // Parse the low surrogate
        // Next 4 characters should be hex digits making up the low surrogate
        for (int i = 0; i < 4; ++i) { 
            char ch = context.consume();

            if (!std::isxdigit(ch)) { // check for hex again
                ErrorHandler::getInstance().handleError(
                    ErrorCode::InvalidUnicode,
                    std::string(1, ch),
                    context.line(),
                    context.column()
                );
                return codepoint;
            }
            hexStr.push_back(ch);
        }
        
        uint32_t lowSurrogate = std::stoul(hexStr, nullptr, 16);
        if (lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF) {
            ErrorHandler::getInstance().handleError(
                ErrorCode::InvalidUnicode,
                "Invalid low surrogate",
                context.line(),
                context.column()
            );
            return codepoint;
        }
        
        // Convert surrogate pair to final codepoint (this is the bit manipulation part)
        // The formula for combining a high surrogate (in the range 0xD800 to 0xDBFF)
        // and a low surrogate (in the range 0xDC00 to 0xDFFF) is as follows:
        // The final codepoint = 0x10000 + ((highSurrogate - 0xD800) << 10) + (lowSurrogate - 0xDC00)
        // This shifts the high surrogate by 10 bits to the left and adds it to the low surrogate.
        // The offset 0x10000 is added to generate the correct Unicode codepoint for characters
        // beyond the Basic Multilingual Plane (BMP).
        return 0x10000 + ((codepoint - 0xD800) << 10) + (lowSurrogate - 0xDC00);
    } 
    // Check for invalid lone low surrogate
    else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
        ErrorHandler::getInstance().handleError(
            ErrorCode::InvalidUnicode,
            "Unexpected low surrogate",
            context.line(),
            context.column()
        );
    }
    
    return codepoint;
}

std::u8string encodeUTF8(uint32_t codepoint, int line, int column) {
    std::u8string res;
    
    // 1 byte UTF-8: 0xxxxxxx
    // Used for codepoints 0x0000-0x007F (0-127)
    if(codepoint <= 0x7F) {
        res.push_back(static_cast<char8_t>(codepoint));
    }

    // 2 byte UTF-8: 110xxxxx 10xxxxxx
    // Used for codepoints 0x0080-0x07FF (128-2047)
    else if(codepoint <= 0x7FF) {
        // First byte: 110xxxxx
        // 0xC0 = 11000000
        // >> 6 puts the top 5 bits after the 110 prefix
        res.push_back(static_cast<char8_t>(0xC0 | (codepoint >> 6)));

        // Second byte: 10xxxxxx
        // 0x80 = 10000000
        // & 0x3F (00111111) masks off the bottom 6 bits
        res.push_back(static_cast<char8_t>(0x80 | (codepoint & 0x3F)));
    }

    // Three byte UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
    // Used for codepoints 0x0800-0xFFFF (2048-65535)
    else if(codepoint <= 0xFFFF) {
        // First byte: 110xxxxx
        // 0xC0 = 11000000
        // >> 6 puts the top 5 bits after the 110 prefix
        res.push_back(static_cast<char8_t>(0xC0 | (codepoint >> 6)));

        // Second byte: 10xxxxxx
        // 0x80 = 10000000
        // & 0x3F (00111111) masks off the bottom 6 bits
        res.push_back(static_cast<char8_t>(0x80 | ((codepoint >> 6) & 0x3F)));

        // Third byte: 10xxxxxx
        // & 0x3F masks to get the bottom 6 bits
        res.push_back(static_cast<char8_t>(0x80 | (codepoint & 0x3F)));
    }

    // Four byte UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    // Used for codepoints 0x10000-0x10FFFF (65536-1114111)
    else if (codepoint <= 0x10FFFF) {
        // First byte: 11110xxx
        // 0xF0 = 11110000
        // >> 18 puts the top 3 bits after the 11110 prefix
        res.push_back(static_cast<char8_t>(0xF0 | (codepoint >> 18)));
        
        // Second byte: 10xxxxxx
        // >> 12 puts the next 6 bits in position
        // & 0x3F masks to get just those 6 bits
        res.push_back(static_cast<char8_t>(0x80 | ((codepoint >> 12) & 0x3F)));
        
        // Third byte: 10xxxxxx
        // >> 6 puts the next 6 bits in position
        // & 0x3F masks to get just those 6 bits
        res.push_back(static_cast<char8_t>(0x80 | ((codepoint >> 6) & 0x3F)));

        // Fourth byte: 10xxxxxx
        // & 0x3F masks to get the bottom 6 bits
        res.push_back(static_cast<char8_t>(0x80 | (codepoint & 0x3F)));
    }

    else {
        ErrorHandler::getInstance().handleError(
            ErrorHandler::ErrorCode::InvalidUnicode,
            "Codepoint out of range",
            line,
            column
        );
    }

    return res;
}


std::unique_ptr<JsonValue> JsonParser::parseArray(Context& context){
    JsonValue::JsonArray arr;
    bool expectingValue = true;
    while (true){
        context.skipWhitespace();

        char ch = context.peek();

        if (ch == ']') { // Closing array
            context.consume(); 
            if (expectingValue && !arr.empty()) {
                ErrorHandler::getInstance().handleError(
                    ErrorCode::UnexpectedToken,
                    "Trailing comma before closing array.",
                    context.line(),
                    context.column()
                );
            }
            return std::make_unique<JsonValue>(std::move(arr));
        }

        if (ch == ',') { // Comma separating values
            context.consume();
            if (expectingValue) {
                ErrorHandler::getInstance().handleError(
                    ErrorCode::UnexpectedToken,
                    "Unexpected comma in array.",
                    context.line(),
                    context.column()
                );
            }
            expectingValue = true; 
            continue;
        }

        // Parsing value logic
        if (expectingValue) {
            auto value = parseValue(context);
            if (!value) {
                ErrorHandler::getInstance().handleError(
                    ErrorCode::UnexpectedToken,
                    "Invalid value in array.",
                    context.line(),
                    context.column()
                );
            }
            arr.push_back(std::move(*value));
            expectingValue = false;
        } else {
            // If not expecting a value but get an unexpected token
            ErrorHandler::getInstance().handleError(
                ErrorCode::UnexpectedToken,
                "Expected ',' or ']' in array.",
                context.line(),
                context.column()
            );
        }
    }
}


JsonParser::Context::Context(std::ifstream& file)
    : m_file(file), m_line(1), m_column(0)  {
    if (!m_file.is_open()) {
        ErrorHandler::getInstance().handleError(ErrorHandler::ErrorCode::FileNotOpen, "", m_line, m_column);
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
        ErrorHandler::getInstance().handleError(ErrorCode::UnexpectedEOF, "", m_line, m_column);
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
