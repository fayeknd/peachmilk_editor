#include "shader.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../windowmanager/WindowManager.hpp"
#include "ShaderManager.hpp"
#include "../system/file.hpp"

//#define DEBUG

void Shader::use() {
    ShaderManager::get().s_activeShader = m_shaderID;
    glUseProgram(m_shaderID);
}

Shader* Shader::createShader(const char* shFilePath, const char* shaderName) {

	std::string vshCode = "";
    std::string fshCode = "";
    std::string gshCode = "";
    std::ifstream shFile;

    bool hasGshHeader = false;

    std::string filePath = File::getWorkingDirectory();
    filePath += shFilePath;
    if (File::fileExists(filePath)) {

        shFile.open(filePath);

        if (!shFile) {
            std::cout << "Shader path '" << filePath << "' was not able to be opened." << std::endl;
            assert("Shader not found!");
            return nullptr;
        }

        std::string line;
        std::string* writeBuf;
        int index = 0;

        while (std::getline(shFile, line)) {
            if (line != s_shaderPrefix && index == 0) {
                std::cout << "WARNING : Shader file does not begin with " << s_shaderPrefix << "!\nAssuming it is a compatiable shader" << std::endl;
            }
            if (line == "#VERTEX") {
                writeBuf = &vshCode;
                line = "#version 460 core\n";
            }
            if (line == "#FRAGMENT") {
                writeBuf = &fshCode; 
                line = "#version 460 core\n";
            }
            if (line == "#GEOMETRY") {
                writeBuf = &gshCode;
                line = "#version 460 core\n";
                hasGshHeader = true;
            }
            if (writeBuf != nullptr && index != 0) {
#ifdef DEBUG
                std::cout << "[Buffer: " << ((writeBuf == &fshCode) ? "fshCode" : "vshCode") << "] Line" << index << ":    " << line << std::endl;
#endif
                *writeBuf += line + "\n";
            }
            index++;
        }
    }
    else {
        std::cout << "Shader path '" << filePath << "' was not found." << std::endl;
        assert("Shader not found!");
        return nullptr;
    }

    m_vsh = glCreateShader(GL_VERTEX_SHADER);
    m_fsh = glCreateShader(GL_FRAGMENT_SHADER);

    stripUnicode(vshCode);
    stripUnicode(fshCode);

    const char* vsh_c = vshCode.c_str();
    const char* fsh_c = fshCode.c_str();
    const char* gsh_c = gshCode.c_str();
    glShaderSource(m_vsh, 1, &vsh_c, NULL);
    glShaderSource(m_fsh, 1, &fsh_c, NULL);


    glCompileShader(m_vsh);
    glCompileShader(m_fsh);

    checkCompileErrors(m_vsh);
    checkCompileErrors(m_fsh);

    if (hasGshHeader) {
        m_gsh = glCreateShader(GL_GEOMETRY_SHADER);
        stripUnicode(gshCode);
        glShaderSource(m_gsh, 1, &gsh_c, NULL);
        glCompileShader(m_gsh);
        checkCompileErrors(m_gsh);
    }

    m_shaderID = glCreateProgram();

    glAttachShader(m_shaderID, m_vsh);
    glAttachShader(m_shaderID, m_fsh);

    if (hasGshHeader) {
        glAttachShader(m_shaderID, m_gsh);
    }

    glLinkProgram(m_shaderID);

#ifdef DEBUG
    std::cout << "Shader source log:\n------------------------------------------------------\n";
    if (hasGshHeader) {
        std::cout << vsh_c << "\n------------------------------------------------------\n" << fsh_c << "\n------------------------------------------------------\n" << gsh_c << "\n------------------------------------------------------" << std::endl;
        std::cout << "SHADER " << m_vsh << ", " << m_fsh << " AND " << m_gsh << " SUCCESSFULLY LINKED INTO SHADER PROGRAM " << m_shaderID << "." << std::endl;
    }
    else {
        std::cout << vsh_c << "\n------------------------------------------------------\n" << fsh_c << "\n------------------------------------------------------" << std::endl;
        std::cout << "SHADER " << m_vsh << " AND " << m_fsh << " SUCCESSFULLY LINKED INTO SHADER PROGRAM " << m_shaderID << "." << std::endl;
    }
#endif
    glDeleteShader(m_vsh);
    glDeleteShader(m_fsh);
    if (hasGshHeader) glDeleteShader(m_gsh);

    m_shaderName = shaderName;

    return this;

}

void Shader::registerShader() {
    ShaderManager::get().s_shaderList.push_back(this);
}
void Shader::unregisterShader() {
    for (int i = 0; i < ShaderManager::get().s_shaderList.size(); i++) {
        if (ShaderManager::get().s_shaderList[i] == this) {
            ShaderManager::get().s_shaderList.erase(ShaderManager::get().s_shaderList.begin() + i);
            return;
        }
    }
    glDeleteShader(m_shaderID);
}

bool Shader::checkCompileErrors(unsigned int shader)
{
	int success;
	char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "SHADER COMPILATION ERROR" << std::endl;
        std::cout << infoLog << std::endl;

        assert("Critical compilation errors!");

        return false;
    }
#ifdef DEBUG
    else
        std::cout << "SHADER " << shader << " COMPILED SUCCESSFULLY" << std::endl;
#endif
    return true;
}