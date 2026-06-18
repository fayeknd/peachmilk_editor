#pragma once
#include "../headers.h"
#include "shader.hpp"

// im only really using this to store all the shaders in al ist lol

class ShaderManager {
public:

    static ShaderManager& get() {
        static ShaderManager instance;
        return instance;
    }

    std::vector<Shader*> s_shaderList;
    int s_activeShader;

};