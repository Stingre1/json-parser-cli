#include <iostream>
#include <fstream>
#include <cassert>
#include "./sources/JsonParser.h"
#include <filesystem>

/**
 * TODO: figure tf out of cmake stil...
 */

inline bool validateArgCount(int argCount) {
    if (argCount < 2) {
        std::cerr << "No arguments. Please add the .json file as an argument.\n";
        return false;
    } else if (argCount > 2) {
        std::cerr << "Too many arguments.\n";
        return false;
    }
    return true;
}

inline bool hasJSONExtension(const std::filesystem::path& path) {
    const std::string extension = ".json";
    const std::string pathStr = path.string();
    const int nameSize = pathStr.size(), extSize = extension.size();
    
    // Compare the last few characters
    return nameSize >= extSize &&
           !(pathStr.compare(nameSize - extSize, extSize, extension));
}

int main(int argc, char const *argv[]) {
    if (!validateArgCount(argc)) { return -1; }
    
    std::filesystem::path path = argv[1];
    if (!hasJSONExtension(path)) {
        std::cerr << "Invalid extension name. Must have .json extension\n";
        return -1;
    }

    // Parse JSON file
    JsonParser parser;
    parser.parse(path);

    std::cout << "end" << std::endl; 
    return 0;
}
