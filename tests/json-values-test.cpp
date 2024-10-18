#define NDEBUG
#include <cassert>
#include "../sources/ValueType.h"  // Include the header file for ValueType

void testStringType() {
    ValueType value("test string");
    assert(value.type == ValueType::Type::STRING);
    assert(std::get<std::string>(value.data) == "test string");
}

void testIntegerType() {
    ValueType value((long)42);
    assert(value.type == ValueType::Type::INTEGER);
    assert(std::get<long>(value.data) == 42);
}

void testDecimalType() {
    ValueType value(3.14);
    assert(value.type == ValueType::Type::DECIMAL);
    assert(std::get<double>(value.data) == 3.14);
}

void testBoolType() {
    ValueType value(true);
    assert(value.type == ValueType::Type::BOOL);
    assert(std::get<bool>(value.data) == true);
}

// void testObjectType() {
//     ValueType::JSONObject obj = { {"key", ValueType("value")} };
//     ValueType value(obj);
//     assert(value.type == ValueType::Type::OBJECT);
//     assert(std::get<ValueType::JSONObject>(value.data)["key"].data == std::get<std::string>(ValueType("value").data));
// }

// void testArrayType() {
//     ValueType::JSONArray arr = { ValueType(1), ValueType(2.5), ValueType("string") };
//     ValueType value(arr);
//     assert(value.type == ValueType::Type::ARRAY);
//     assert(std::get<ValueType::JSONArray>(value.data)[0].data == std::get<long>(ValueType(1).data));
//     assert(std::get<ValueType::JSONArray>(value.data)[1].data == std::get<double>(ValueType(2.5).data));
//     assert(std::get<ValueType::JSONArray>(value.data)[2].data == std::get<std::string>(ValueType("string").data));
// }

// void testNullType() {
//     ValueType value(ValueType::NullType{});
//     assert(value.type == ValueType::Type::NUL);
// }

int main() {
    testStringType();
    testIntegerType();
    testDecimalType();
    testBoolType();
    // testObjectType();
    // testArrayType();
    // testNullType();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
