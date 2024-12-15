#include "ValueTypes.h"
#include <map>
#include <vector>

ValueTypes::ValueTypes(const std::string& str) : type(Type::STRING), value(str) {}
ValueTypes::ValueTypes(long num) : type(Type::INTEGER), value(num) {}
ValueTypes::ValueTypes(double decimal) : type(Type::DECIMAL), value(decimal) {}
ValueTypes::ValueTypes(bool b): type(Type::BOOL), value(b) {}
ValueTypes::ValueTypes(JsonObject& obj) : type(Type::OBJECT), value(obj) {}
ValueTypes::ValueTypes(JsonArray& arr) : type(Type::ARRAY), value(arr) {}
ValueTypes::ValueTypes(NullType nul) : type(Type::NUL), value(nul) {}
