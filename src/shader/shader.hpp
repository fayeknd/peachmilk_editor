#pragma once
#include "../headers.h"
#include <string>
#include <type_traits>
#include "../render/texture.hpp"


class Shader {
private:
    int m_shaderID;
    const char* m_vshSource;
    const char* m_fshSource;
    const char* m_gshSource;

    static inline const char* s_shaderPrefix = "#EngineShader";

    unsigned int m_vsh;
    unsigned int m_fsh;
    unsigned int m_gsh = -1;

    void stripUnicode(std::string& str)
	{
		str.erase(std::__remove_if(str.begin(), str.end(), [](char c) {return !(c >= 0 && c < 128); }), str.end());
	}

    void registerShader();
    void unregisterShader();

public:

    static inline float m_wireframeCol[4]{1, 0, 1, 1};
    static inline float m_highlightCol[4]{0, 1, 0, 0.3f};
    static inline float m_selectBoxCol[4]{0.34, 0.34, 1, 0.3f};
    static inline float m_clearCol[4]{0.172f, 0.172f, 0.172f, 1.0f};
    static inline float m_wireframeWidth = 1.3f;

    const char* m_shaderName;

    Shader(const char* shFilePath, const char* shaderName = "Unnamed Shader") {
        createShader(shFilePath, shaderName);
        registerShader();
    }
    ~Shader() {
        unregisterShader();
    }

    int getVshID() { return m_vsh; }
    int getFshID() { return m_fsh; }
    int getGshID() { return m_gsh; }
    int getShaderID() { return m_shaderID; }
    Shader* createShader(const char* shFilePath, const char* shaderName = "Unnamed Shader");
    bool checkCompileErrors(unsigned int shader);

    void use();

    void setMat4(glm::mat4 mat, const char* uniformLoc) {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderID, uniformLoc), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setTexture(Texture* texture, const char* uniformLoc) {
        glUniform1i(glGetUniformLocation(m_shaderID, uniformLoc), texture->getID());
    }
    void setBool(bool b, const char* uniformLoc) {
        glUniform1i(glGetUniformLocation(m_shaderID, uniformLoc), b);
    }
    void setVec4(glm::vec4 vec, const char* uniformLoc) {
        glUniform4f(glGetUniformLocation(m_shaderID, uniformLoc), vec.x, vec.y, vec.z, vec.w);
    }
    void setVec3(glm::vec3 vec, const char* uniformLoc) {
        glUniform3f(glGetUniformLocation(m_shaderID, uniformLoc), vec.x, vec.y, vec.z);
    }

};