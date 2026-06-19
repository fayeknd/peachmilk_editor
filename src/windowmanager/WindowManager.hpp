#pragma once
#include <iostream>
#include "../headers.h"

class WindowManager {
private:

    int m_gl_major;
    int m_gl_minor;

    static inline WindowManager* instance = nullptr;

    FullscreenMode m_fullscreenMode;

    glm::vec2 m_size_last;
    float m_frame = 0;

public:

    bool virtual init(int width, int height, const char* title, window_hint_func window_hints, int gl_major = 4, int gl_minor = 6);
    void virtual begin();
    void virtual default_window_hints(int gl_major, int gl_minor);

    FullscreenMode getFullscreenMode() { return m_fullscreenMode; }
    void setFullscreenMode(FullscreenMode fsm);
    float currentFrame() { return m_frame; }

    glm::vec2 m_size;
    glm::vec2 m_viewportSize; 

    float m_targetFps = TARGET_FPS;

    static WindowManager* get() {
        return instance;
    }

    bool m_resize_viewport_with_window = true;
    
    int getGLMajor() { return m_gl_major; }
    int getGLMinor() { return m_gl_minor; }

    GLFWwindow* m_wnd;
    virtual void create(int width, int height, const char* title, init_func pre_init_func = nullptr, window_hint_func window_hints = nullptr, int gl_major = 4, int gl_minor = 6);
    virtual void update();

    virtual void onClose() {};
    virtual void triggerUpdates();
    virtual void triggerDraws();
    virtual void setFramebufferCallbacks();
    void centerWindow(); 

};