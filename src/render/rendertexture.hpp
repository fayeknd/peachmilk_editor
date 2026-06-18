#pragma once
#include "../headers.h"

class RenderTexture {
private:
    unsigned int m_ID;
public:
    void create(int x, int y, GLuint format = GL_RGB, GLuint filterMode = GL_NEAREST);
    unsigned int getID() { return m_ID; }
};
