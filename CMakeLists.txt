cmake_minimum_required(VERSION 3.31.0-rc1)
project(json-parser VERSION 0.1.0 LANGUAGES C CXX)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "sources/*.cpp")

add_executable(json-parser 
    main.cpp
    sources/JsonParser.cpp
    sources/ValueType.cpp
)

include_directories(sources)

include(CTest)
enable_testing()
include(CPack)
