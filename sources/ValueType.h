#pragma once
#include <variant>
#include <iostream>
#include <map>
#include <vector>

class ValueType;

class ValueType {
public:
    // referred https://www.json.org/json-en.html for types
    enum class Type {   
      STRING,
      INTEGER,
      DECIMAL,
      BOOL,
      OBJECT,
      ARRAY,
      NUL
    };
    
    using JSONObject = std::map<std::string, ValueType>;
    using JSONArray = std::vector<ValueType>;
    using NullType = std::monostate;
    
public:
    Type type;
    std::variant<std::string, long, double, bool, JSONObject, JSONArray, NullType> data; 
public:
    ValueType(const std::string& str);
    ValueType(long num);
    ValueType(double decimal);
    ValueType(bool b);
    ValueType(JSONObject& obj);
    ValueType(JSONArray& arr);
    ValueType(NullType nul);
};

