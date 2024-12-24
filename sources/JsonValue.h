#pragma once
#include <variant>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "ErrorHandler.h"

class JsonValue {
public:
    // see: https://www.json.org/json-en.html
    enum class Type {   
      STRING,
      INTEGER,
      DECIMAL,
      BOOL,
      OBJECT,
      ARRAY,
      NUL
    };
    
    using JsonObject = std::map<std::u8string, JsonValue>;
    using JsonArray = std::vector<JsonValue>;
    using NullType = std::monostate;
    
    using Value = std::variant<std::u8string, long, double, bool, JsonObject, JsonArray, NullType>;

private:
    Type m_type;
    Value m_value;

public:
    JsonValue(const std::u8string& str);
    JsonValue(long num);
    JsonValue(double decimal);
    JsonValue(bool b);
    JsonValue(JsonObject& obj);
    JsonValue(JsonArray& arr);
    JsonValue(NullType nul);

    const std::u8string& getString() const;

    Type getType() const { return m_type; }
};

