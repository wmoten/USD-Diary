
#include "../include/cli_processor.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
namespace fs = std::__fs::filesystem;



void raiseError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}


void printUsage() {
    raiseError("Usage: ./scope_reparent_run <usdFilePath> [scopeName]");
}

bool extractCommandLineArguments(int argc, char* argv[], std::string& usdFilePath, std::string& scopeName) {
    if (argc < 2 || argc > 3) {
        printUsage();
        return false;
    }

    usdFilePath = argv[1];

    // did the user enter a scope name?
    if (argc == 3) {
        // Yes!
        scopeName = argv[2];
    } else {
        // No :(
        std::cout << "Please enter the name of the scope prim you'd like to create: ";
        std::getline(std::cin, scopeName);
    }

    return true;
}


std::string deriveNewFilePath(const std::string& originalFilePath, const std::string& scopeName) {
    // Get the absolute path of the original file
    const fs::path absolutePath = fs::absolute(originalFilePath);

    // Get the directory and base filename of the absolute file path
    const std::string directory = absolutePath.parent_path().string();
    const std::string baseName = scopeName + "." + absolutePath.extension().string().substr(1);

    // Construct the result file path by appending the scope name and original extension to the directory path
    std::string resultFilePath = directory + "/" + baseName;
    return resultFilePath;
}


std::string getFileWithPrefix(const std::string& filePath) {
    fs::path path(filePath);
    std::string filename = path.stem().string();
    std::string extension = path.extension().string();
    return "./" + filename + extension;
}
