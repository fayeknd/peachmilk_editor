#pragma once
#include <iostream>

class File {
public:
    static std::string getWorkingDirectory();
    static std::string getExtension(std::string file);
    static std::string getFullPath(std::string file);
    static bool fileExists (const std::string& file);
};