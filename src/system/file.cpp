#include "file.hpp"
#include <filesystem>
#include <fstream>

std::string File::getWorkingDirectory() {
    std::ostringstream file;
    std::string file_s;
    
    file << std::filesystem::current_path();
    file_s = file.str();

    file_s.erase(file_s.begin());
    file_s.erase(file_s.end() - 1);
    file_s += "/";
    
   //std::cout << file_s << std::endl;

    return file_s;
}

std::string File::getExtension(std::string file) {
    std::string s(file);
    size_t index = s.find_last_of('.');
    return s.substr(index + 1, s.size()).c_str();
}

bool File::fileExists(const std::string& file) {
    std::ifstream f(file.c_str());
    return f.good();
}

std::string File::getFullPath(std::string file) {
    return std::filesystem::canonical(file).string();
}