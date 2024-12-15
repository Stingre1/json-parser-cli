#pragma once
#include <variant>
#include <iostream>
#include <map>
#include <vector>
#include <string>

class ValueTypes {
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
    
    using JsonObject = std::map<std::u8string, ValueTypes>;
    using JsonArray = std::vector<ValueTypes>;
    using NullType = std::monostate;
    
    using JsonValue = std::variant<std::string, long, double, bool, JsonObject, JsonArray, NullType>;
public:
    Type type;
    JsonValue value;
public:
    ValueTypes(const std::string& str);
    ValueTypes(long num);
    ValueTypes(double decimal);
    ValueTypes(bool b);
    ValueTypes(JsonObject& obj);
    ValueTypes(JsonArray& arr);
    ValueTypes(NullType nul);
};

