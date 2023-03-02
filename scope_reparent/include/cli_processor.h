
#ifndef CLI_PROCESSOR_H
#define CLI_PROCESSOR_H
#include <string>

void raiseError(const std::string& message);
void printUsage();
bool extractCommandLineArguments(int argc, char* argv[], std::string& usdFilePath, std::string& scopeName);
std::string deriveNewFilePath(const std::string& originalFilePath, const std::string& scopeName);
std::string getFileWithPrefix(const std::string& filePath);

#endif
