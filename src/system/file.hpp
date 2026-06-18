#pragma once
#include <iostream>

class File {
public:
    static const char* getWorkingDirectory();
    static std::string getExtension(const std::string& file);
    static std::string getExtension(const char* file);
    static std::string getFullPath(const char* file);
    static bool fileExists (const std::string& file);
};