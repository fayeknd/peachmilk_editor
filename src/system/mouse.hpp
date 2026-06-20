#pragma once
#include "../headers.h"

class Mouse {
private:

    glm::vec2 m_pos;
    glm::vec2 m_posLF;
    glm::vec3 m_scrPosLF;
    glm::vec2 m_scrollOffset;

public:

    static Mouse& get() {
        static Mouse instance;
        return instance;
    }

    glm::vec2 s_mouseOffset;

    glm::vec2 posDelta() { return m_pos - m_posLF; };
    glm::vec2 pos() { return m_pos - s_mouseOffset; };
    glm::vec2 rawPos() { return m_pos; }
    glm::vec2 posLastFrame() { return m_posLF; };
    glm::vec2 scrollDelta() { return m_scrollOffset; };
    glm::vec3 screenPos();
    glm::vec3 screenPosLF() { return m_scrPosLF; };
    glm::vec3 rawScreenPos();

    void pollMouse();
    void glfw_updateScrollDelta(glm::vec2 sd) { m_scrollOffset = sd; };

    void createMouseCursorData();
    void setMouseCursor(GLFWcursor* cursor, GLFWwindow* wnd = nullptr);

    GLFWcursor* s_handCursor;
    GLFWcursor* s_defaultCursor;
    GLFWcursor* s_moveCursorNESW;
    GLFWcursor* s_moveCursorNS;
    GLFWcursor* s_moveCursorWE;

    GLFWcursor* s_currentCursor;

};