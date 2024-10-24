#include "ValueType.h"
#include <map>
#include <vector>

ValueType::ValueType(const std::string& str) : type(Type::STRING), data(str) {}
ValueType::ValueType(long num) : type(Type::INTEGER), data(num) {}
ValueType::ValueType(double decimal) : type(Type::DECIMAL), data(decimal) {}
ValueType::ValueType(bool b): type(Type::BOOL), data(b) {}
ValueType::ValueType(JSONObject& obj) : type(Type::OBJECT), data(obj) {}
ValueType::ValueType(JSONArray& arr) : type(Type::ARRAY), data(arr) {}
ValueType::ValueType(NullType nul) : type(Type::NUL), data(nul) {}
