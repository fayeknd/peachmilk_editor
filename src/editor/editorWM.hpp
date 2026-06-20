#pragma once
#include "../windowmanager/WindowManager.hpp"
#include "../editor/editor.hpp"
#include "project.hpp"

class EditorWM : public WindowManager {

public:

    EditorUI * m_editor = nullptr;

    void create(int width, int height, const char* title, init_func pre_init_func = nullptr, window_hint_func window_hints = nullptr, int gl_major = 4, int gl_minor = 6) override;
    void update() override;
    bool createProjectSelection();
    bool init(int width, int height, const char* title, window_hint_func window_hints, int gl_major = 4, int gl_minor = 6) override;
    void default_window_hints(int gl_major, int gl_minor) override;
    void triggerDraws() override;
    void onClose() override;
    std::vector<Project*> getProjectPaths();
};