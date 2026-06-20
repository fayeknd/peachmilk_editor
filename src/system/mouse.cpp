#include "../windowmanager/WindowManager.hpp"
#include "mouse.hpp"

void Mouse::pollMouse() {
    double x, y; 
    m_posLF = m_pos;
    m_scrPosLF = screenPos();
    glfwGetCursorPos(WindowManager::get()->m_wnd, &x, &y);
    m_pos = glm::vec2(x, y);
}

glm::vec3 Mouse::screenPos() {
    glm::vec2 mousePos = Mouse::get().pos() - WindowManager::get()->m_size / 2.0f;
    return {mousePos.x, -mousePos.y, 0};
}
glm::vec3 Mouse::rawScreenPos() {
    glm::vec2 mousePos = Mouse::get().rawPos() - WindowManager::get()->m_size / 2.0f;
    return {mousePos.x, -mousePos.y, 0};
}

void Mouse::createMouseCursorData() {
    s_defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    s_handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    s_moveCursorNESW = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    s_moveCursorNS = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    s_moveCursorWE = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);

    s_currentCursor = s_defaultCursor;
}

void Mouse::setMouseCursor(GLFWcursor* cursor, GLFWwindow* wnd) {
    if (cursor == s_currentCursor) return; 
    if (!wnd) wnd = WindowManager::get()->m_wnd;
    glfwSetCursor(wnd, cursor);
    s_currentCursor = cursor;
}