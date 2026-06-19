#include "WindowManager.hpp"
#include "../shader/ShaderManager.hpp"
#include "../system/time.hpp"
#include "../render/sprite.hpp"
#include "../render/vertex.hpp"
#include "../render/entity.hpp"
#include "../render/camera.hpp"
#include "../system/mouse.hpp"
#include "../system/audio.hpp"

void fb_resize_callback(GLFWwindow* window, int width, int height) {
    if (WindowManager::get()->m_resize_viewport_with_window) {
        glViewport(0, 0, width, height);
        WindowManager::get()->m_viewportSize = glm::vec2(width, height);
    }
    
    WindowManager::get()->m_size = glm::vec2(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Mouse::get().glfw_updateScrollDelta(glm::vec2(xoffset, yoffset));
}

void WindowManager::centerWindow() {
    float x, y;
    const GLFWvidmode* vm = glfwGetVideoMode(glfwGetPrimaryMonitor());
    
    x = (vm->width - m_size.x) / 2;
    y = (vm->height - m_size.y) / 2;

    glfwSetWindowPos(m_wnd, x, y);
}

void WindowManager::setFullscreenMode(FullscreenMode fsm) {
    if (fsm == m_fullscreenMode) return;
    switch (fsm) {
        case Fullscreen: {
            m_size_last = m_size;
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(m_wnd, glfwGetPrimaryMonitor(), 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
            m_size = glm::vec2(vidmode->width, vidmode->height);
            break;
        }
        
        case Windowed: {
            glfwSetWindowAttrib(m_wnd, GLFW_DECORATED, true);
            if (m_fullscreenMode == FullscreenMode::Fullscreen) {
                glfwSetWindowMonitor(m_wnd, NULL, 0, 0, m_size_last.x, m_size_last.y, 0);
                m_size = m_size_last;
            }
            break;
        }

        case Borderless: {
            glfwSetWindowAttrib(m_wnd, GLFW_DECORATED, false);
            if (m_fullscreenMode == FullscreenMode::Fullscreen) {
                glfwSetWindowMonitor(m_wnd, NULL, 0, 0, m_size_last.x, m_size_last.y, 0);
                m_size = m_size_last;
            }
            break;
        }

    }
    m_fullscreenMode = fsm;
}

void WindowManager::default_window_hints(int gl_major, int gl_minor) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void WindowManager::setFramebufferCallbacks() {
    glfwSetFramebufferSizeCallback(m_wnd, fb_resize_callback);
    glfwSetScrollCallback(m_wnd, scroll_callback);
}

bool WindowManager::init(int width, int height, const char* title, window_hint_func window_hints, int gl_major, int gl_minor) {

    if (instance != nullptr) {
        std::cout << "ERROR : WindowManager already exists at " << instance << "!";
        return false; 
    }
    instance = this;
    glfwInit();
    
    if (window_hints != nullptr) {
        window_hints(gl_major, gl_minor);
    }
    else {
        default_window_hints(gl_major, gl_minor);
    }
    
    m_wnd = glfwCreateWindow(width, height, title, NULL, NULL);
    
    m_gl_major = gl_major;
    m_gl_minor = gl_minor;

    if (!m_wnd) {
        std::cout << "GLFW Window creation failed for unknown reason." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_wnd);
    setFramebufferCallbacks();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD Load Procedure failed for unknown reason." << std::endl;
        return false;
    }

    fb_resize_callback(m_wnd, width, height);
    setFullscreenMode(FullscreenMode::Windowed);
    Mouse::get().createMouseCursorData();

    return true;

}

void WindowManager::triggerUpdates() {
    for (int i = 0; i < GameLevel::s_loadedEntities.size(); i++) {
        GameLevel::s_loadedEntities.at(i)->update();
    }
}

void WindowManager::triggerDraws() {
    for (int i = GameLevel::s_loadedEntities.size() - 1; i >= 0; --i) {
        GameLevel::s_loadedEntities.at(i)->draw();
    }
}

void WindowManager::update() {
    triggerUpdates();
}

void WindowManager::begin() {
    Time::get().m_deltaTimer.start();

    while (!glfwWindowShouldClose(m_wnd)) {
        if (Time::deltaTimeNow() >= ((double)1 / Math::clamp((double)m_targetFps, (double)MIN_FPS, (double)MAX_FPS))) {
            Time::resetDeltaTimer();

            m_frame++;
            AudioManager::get().update(Camera::mainCamera->transform, Camera::mainCamera->m_isOrtho);
            Mouse::get().pollMouse();
            update();
            triggerDraws();
            Mouse::get().glfw_updateScrollDelta(glm::vec2(0));
            glfwSwapBuffers(m_wnd);
            glfwPollEvents();
        }
    }
}

void WindowManager::create(int width, int height, const char* title, init_func pre_init_func, window_hint_func window_hints, int gl_major, int gl_minor) {
    if (pre_init_func != nullptr)
        pre_init_func();
    if (init(width, height, title, window_hints, gl_major, gl_minor)) {
        // begin main game logic loop
        begin();
    }
    onClose();
    glfwDestroyWindow(m_wnd);
    glfwTerminate();
    delete this;
}