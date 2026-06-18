#include "../headers.h"
#include "rendertexture.hpp"

class Framebuffer {
private:

    static inline unsigned int s_boundFB = -1;
    unsigned int m_ID;
    bool m_initialised = false;
    glm::vec2 m_size;

public:

    glm::vec2 size() { return m_size; }
    void setSize(glm::vec2 newSize);
    unsigned int getID() { return m_ID; }
    static inline unsigned int getActiveID() { return s_boundFB; }
    bool use();
    static bool useDefault();
    void create(int x, int y, GLuint format = GL_RGB, GLuint filterMode = GL_NEAREST);
    bool isInitialised(); 
    RenderTexture m_renderTexture;

    ~Framebuffer() {
        if (m_initialised) glDeleteFramebuffers(1, &m_ID);
    }
};