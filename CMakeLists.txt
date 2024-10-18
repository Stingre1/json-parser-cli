cmake_minimum_required(VERSION 3.31.0-rc1)
project(json-parser VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB SOURCES "sources/*.cpp")

add_executable(json-parser 
    main.cpp
    sources/JsonParser.cpp
    sources/ValueType.cpp
)

include_directories(sources)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
