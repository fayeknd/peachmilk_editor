#include "editorWM.hpp"
#include "../shader/ShaderManager.hpp"
#include "../system/time.hpp"
#include "../render/sprite.hpp"
#include "../render/vertex.hpp"
#include "../render/entity.hpp"
#include "../render/camera.hpp"
#include "../system/mouse.hpp"
#include "../editor/editor.hpp"
#include "../audio/audioclip.hpp"
#include "../render/text/text.hpp"

void EditorWM::create(int width, int height, const char* title, init_func pre_init_func, window_hint_func window_hints, int gl_major, int gl_minor) {
    WindowManager::create(width, height, title, pre_init_func, window_hints, gl_major, gl_minor);
}

void EditorWM::update() {
    WindowManager::update();
    /*
    glm::vec3 camPos = Camera::mainCamera->transform.getPosition();
    camPos.z = 0;
    glm::vec2 mousePos = Mouse::get().pos() - WindowManager::get()->m_size / 2.0f;
    glm::vec3 mousePos_3 = {mousePos.x, -mousePos.y, 0};

    spr.transform.setPosition((mousePos_3 / Camera::mainCamera->m_zoomFactor) + camPos);
    */
}

void EditorWM::default_window_hints(int gl_major, int gl_minor) {
    WindowManager::default_window_hints(gl_major, gl_minor);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
}

void EditorWM::triggerDraws() {
    //WindowManager::triggerDraws();
    RenderEntity::drawEntites();
    m_editor->drawUI();
}

bool EditorWM::init(int width, int height, const char* title, window_hint_func window_hints, int gl_major, int gl_minor) {
    if (!WindowManager::init(width, height, title, window_hints, gl_major, gl_minor)) {
        return false;
    }
    m_editor = new EditorUI;
    m_editor->m_destroyOnLoad = false;
    m_editor->m_serialize = false;
    int x, y, n;
    unsigned char* pixels = Texture::getDataFromFileSTBI("game_data/engine.png", &x, &y, &n);
    GLFWimage image[1];
    image[0].width = x,
    image[0].height = y;
    image[0].pixels = pixels;
    glfwSetWindowIcon(m_wnd, 1, image);
    Camera::mainCamera->transform.setLocalPositionZ(5);
    Texture::shouldFlip(true);

    Mesh::s_createUniqueMeshes();

    if (!createProjectSelection()) {
        glfwDestroyWindow(m_wnd);
        glfwTerminate();
        return false;
    }
    glfwSetWindowSize(m_wnd, 1920, 1080);
    centerWindow();
    //setFullscreenMode(FullscreenMode::Borderless);
    m_targetFps = Math::Inifinity();
    //glfwSwapInterval(0);

    Shader* shader = new Shader(("game_data/shaders/testshader.vfsh"));

    glEnable(GL_BLEND);
    glClearColor(0.655f, 0.898f, 0.71f, 1.0f);
    m_editor->_start();
    return true;
}

bool EditorWM::createProjectSelection() {
    glfwSetWindowSize(m_wnd, 600, 400);
    centerWindow();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_wnd, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    bool flag = false;
    bool textFlag = false;
    std::string title_old = glfwGetWindowTitle(m_wnd);
    glfwSetWindowTitle(m_wnd, "Project Manager");
    std::vector<Project*> projects = getProjectPaths();
    char buf[32] {};
    while (!glfwWindowShouldClose(m_wnd)) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int x, y;
        glfwGetWindowSize(m_wnd, &x, &y);
        ImGui::SetNextWindowPos({0,0});
        // fuck you im doing c style casts
        // c style casts from int to float are freaking FINE
        ImGui::SetNextWindowSize({(float)x, (float)y});
        ImGui::Begin("Project Selection", NULL,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoSavedSettings
        );

        const char* _ = "Open or Create new project";
        auto textWidth   = ImGui::CalcTextSize(_).x;

        ImGui::SetCursorPosX((x - textWidth) * 0.5f);
        ImGui::Text(_);
        ImGui::NewLine();
        ImGui::BeginChild("Project Window");
        if (ImGui::Button("Create New")) {
            textFlag = true;
        }
        ImGui::SameLine();
        ImGui::Text("Projects");
        ImGui::NewLine();
        if (textFlag) {
            ImGui::InputText("##name", buf, 32);
            if (glfwGetKey(m_wnd, GLFW_KEY_ENTER) == GLFW_PRESS) {
                textFlag = false;
                Project* newProject = new Project;
                newProject->createProject(buf);
                projects.push_back(newProject);
                for (int i = 0; i < 32; i++) {
                    buf[i] = {};
                }
            }
        }
        if (projects.size() == 0) {
            ImGui::Text("No Projects");
        }
        else {
            for (auto p : projects) {
                if (ImGui::Button(p->m_projectTitle.c_str())) {
                    flag = true;
                    Project::setLoadedProject(p);
                }
            }
        }

        ImGui::EndChild();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_wnd);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        if (flag == true) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwSetWindowTitle(m_wnd, ("Project: " + Project::get()->m_projectTitle + " [" + title_old + "]").c_str());
            break;
        }
    }
    for (auto p : projects) {
        if (Project::get() != p) delete p;
    }
    Project::get()->loadProject();
    if (!flag)
    {
        // the program was closed naturally and therefore
        // imgui wasn't properly terminated.
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    return flag;
}

std::vector<Project*> EditorWM::getProjectPaths() {
    std::vector<Project*> _;
    std::string path = std::string(File::getWorkingDirectory()) + std::string("//") + PROJECTS_RC;
    if (!File::fileExists(path)) {
        std::filesystem::create_directory(path);
    }
    for (auto entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            for (auto entry2 : std::filesystem::directory_iterator(entry.path())) {
                std::filesystem::path p(entry2);
                if (p.filename() == ".pinfo") {
                    Project* p = new Project;
                    p->deserializeProjectInfo(entry2.path().string());
                    _.push_back(p);
                }
            }
        }
    }
    return _;
}

void EditorWM::onClose() {
    Project::get()->saveProject();
}
