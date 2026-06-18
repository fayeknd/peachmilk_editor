#include "framebuffer.hpp"

void Framebuffer::create(int x, int y, GLuint format, GLuint filterMode) {
    if (m_initialised) return;

    m_size = {x, y};
    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    m_renderTexture.create(x, y, format, filterMode);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture.getID(), 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) abort();
    
    m_initialised = true;
}

void Framebuffer::setSize(glm::vec2 newSize) {
    glBindTexture(GL_TEXTURE_2D, m_renderTexture.getID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture.getID(), 0);
    m_size = newSize;
}

bool Framebuffer::use() {
    if (m_ID == s_boundFB) return false;
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    s_boundFB = m_ID;
    return true;
}

bool Framebuffer::useDefault() {
    if (s_boundFB == 0) return false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    s_boundFB = 0;
    return true;
}