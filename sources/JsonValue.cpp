#include "JsonValue.h"
#include "ErrorHandler.h"
#include <map>
#include <vector>


JsonValue::JsonValue(const std::u8string& str) 
    : m_type(Type::STRING), m_value(str) {}

JsonValue::JsonValue(long num) 
    : m_type(Type::INTEGER), m_value(num) {}

JsonValue::JsonValue(double decimal) 
    : m_type(Type::DECIMAL), m_value(decimal) {}

JsonValue::JsonValue(bool b) 
    : m_type(Type::BOOL), m_value(b) {}

JsonValue::JsonValue(JsonObject& obj)
    : m_type(Type::OBJECT), m_value(obj) {}

JsonValue::JsonValue(JsonArray& arr) 
    : m_type(Type::ARRAY), m_value(arr) {}

JsonValue::JsonValue(NullType nul) 
    : m_type(Type::NUL), m_value(nul) {}


const std::u8string& JsonValue::getString() const {
    if(m_type != Type::STRING) {
        auto& errorHandler = ErrorHandler::getInstance();
        errorHandler.handleError(ErrorHandler::ErrorCode::TypeMismatch, "Expected a JSON string.", -1, -1);
    }
    return std::get<std::u8string>(m_value);
}
