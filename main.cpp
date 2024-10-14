#include <iostream>
#include <fstream>
// #define NDEBUG
#include <cassert>
#ifdef NDEBUG
    #include "tests/"
#endif NDEBUG


bool validateArgCount(int argCount) {
    if(argCount < 2) {
        std::cerr<<"No arguments. Please add the .json file as an argument.\n";
        return false;
    } else if (argCount > 2) {
        std::cerr<<"Too many arguments.\n";
        return false;
    }
}

bool hasJSONExtension(const std::string& fileName) {
    const std::string extension = ".json";
    const int nameSize = fileName.size(), extSize = extension.size();
    //compare the last 5 characters
    return nameSize >= extSize &&
           !(fileName.compare(nameSize - extSize, extSize, extension)); 
}

int main(int argc, char const *argv[]) {

    if(!validateArgCount(argc)) { return -1; }
    
    std::string fileName = argv[1];
    if(!hasJSONExtension(fileName)) {
        std::cerr<<"Invalid extension name. Must have .json extension\n";
        return -1;
    }

    std::ifstream jsonFile {fileName};

    if(!jsonFile){
        std::cerr<<"File not found error.\n";
        return -1;
    }

    //parse json file




    jsonFile.close();
    return 0;
}
