
#include "editor.hpp"
#include "../windowmanager/WindowManager.hpp"
#include "../render/camera.hpp"
#include "../system/mouse.hpp"
#include "../shader/shader.hpp"
#include "../system/file.hpp"
#include <fstream>

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
    for (i = 0;  i < count;  i++) {
        if (!File::fileExists(paths[i])) {
            std::cout << "Provided file " << paths[i] << " is invalid";
        }
        else {
            Sprite* spr = new Sprite;
            spr->create();
            spr->m_material = new Material;
            spr->m_material->m_diffuseTexture = Texture::createNewTextureFromPath(paths[i], GL_NEAREST);
            spr->setScaleToTexelSize();
        }
    }
}

void EditorUI::start() {

    m_serialize = false;
    m_selectionBox.m_serialize = false;
    m_selectionBox.m_destroyOnLoad = false;
    m_selectionBox.m_material.m_doSerialize = false;
    glm::vec2 size = WindowManager::get()->m_size;
    Camera::mainCamera->m_framebuffer = new Framebuffer;
    Camera::mainCamera->m_framebuffer->create(size.x, size.y);
    Camera::mainCamera->m_serialize = false;
    Camera::mainCamera->m_destroyOnLoad = false;

    m_materialIcon = *Texture::createNewTextureFromPath("editor_data/ui/texture1.mgd");

    m_fileDiag.SetTitle("File Browser");
    m_fileDiag.SetDirectory(File::getWorkingDirectory());
    
    glfwSetDropCallback(WindowManager::get()->m_wnd, drop_callback);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(WindowManager::get()->m_wnd, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    createAxisHandle();
    m_hidden = true;
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

}

float scroll_lf;
float zoom_lf;
bool test = true;

bool EditorUI::drawEntityUI(ScriptableEntity* spr, float xOffset, bool open) {
    //ScriptableEntity* spr = dynamic_cast<Sprite*>(entity);
    std::string tag = "##" + spr->m_entityName;
    ImGui::SetCursorPos({10, ImGui::GetCursorPos().y});
    ImGui::Checkbox(std::string(tag + "__btn").c_str(), &spr->m_visible);
    ImGui::SameLine();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
    ImGui::SetCursorPos({ImGui::GetCursorPos().x - 5.0f + xOffset, ImGui::GetCursorPos().y});
    if (spr->getChildren().size() != 0) {
        flags = ImGuiTreeNodeFlags_OpenOnArrow | !ImGuiTreeNodeFlags_OpenOnDoubleClick;
    }
    if (spr->m_selected) {
        flags = flags | ImGuiTreeNodeFlags_Framed;
        if (spr->getChildren().size() == 0) ImGui::SetCursorPos({ImGui::GetCursorPos().x + 20.0f, ImGui::GetCursorPos().y});
    } 
    bool thisFrameFlag = false;
    bool openFlag = false;
    if (open) {
        ImGui::SetNextItemOpen(true);
    }
    if (ImGui::TreeNodeEx((m_editingText && m_spriteNameEditing == spr) ? tag.c_str() : spr->m_entityName.c_str(), flags)) {
        openFlag = true;
        ImGui::TreePop();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        m_editingText = true;
        m_spriteNameEditing = spr;
        thisFrameFlag = true;
        memset(m_replacementName, 0, 64);
        for (int j = 0; j < spr->m_entityName.size(); j++) {
            m_replacementName[j] = spr->m_entityName[j];
        }
    }
    if (m_editingText && m_spriteNameEditing == spr) {
        ImGui::SameLine();
        //ImGui::SetCursorPos({ImGui::GetCursorPos().x - 20.0f, ImGui::GetCursorPos().y});
        ImGui::SetKeyboardFocusHere(0);
        ImGui::InputText(std::string(tag + "__txt").c_str(), m_replacementName, 64, ImGuiInputTextFlags_AutoSelectAll);
        if (glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_ENTER) == GLFW_PRESS ||
            (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered() && !thisFrameFlag)) {
            m_editingText = false;
            std::string s = m_replacementName;
            m_spriteNameEditing = nullptr;
            
            if (s != "") {
                spr->m_entityName = m_replacementName;
            }
        }
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        if (m_selectedSprites.size() != 0) {
            if (spr != m_selectedSprites[0])
            m_rightClickedObject = spr;
            m_rcoSelectedInUI = true;
        }
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        open = (glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
        if (glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (spr->m_selected) {
                for (int j = 0; j < m_selectedSprites.size(); j++) {
                    if (m_selectedSprites[j] == spr) {
                        m_selectedSprites.erase(m_selectedSprites.begin() + j);
                        break;
                    }
                }
                spr->m_selected = false;
            }
            else {
                m_selectedSprites.push_back(spr);
                spr->m_selected = true; 
            }
        }
        else {
            for (int j = 0; j < m_selectedSprites.size(); j++) {
                m_selectedSprites[j]->m_selected = false;
            }
            m_selectedSprites.clear();
            m_selectedSprites.push_back(spr);
            spr->m_selected = true;
        }
    }
    if (openFlag) {
        for (int i = 0; i < spr->getChildren().size(); i++) {
            drawEntityUI(spr->getChildAt(i), xOffset + 20, open);
        }
    }
    return true;
}

bool EditorUI::drawMaterialPreview(Material* mat, float size, bool inTextureMenu) {
    int id = mat->getID();
    std::string id_s = std::to_string(id);
    ImGui::BeginChild(("##Material " + id_s).c_str(), {size, size + 110}, 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    bool returnFlag = false;
    if (ImGui::CollapsingHeader(("Material Texture " + id_s).c_str(), ImGuiTreeNodeFlags_Leaf)) {
        if (ImGui::ImageButton(("##Material_btn" + id_s).c_str(), mat->m_diffuseTexture->getID(), {size - 5, size - 5}, {0, 1}, {1, 0}, {0,0,0,0}, {mat->m_colour.x, mat->m_colour.y, mat->m_colour.z, mat->m_colour.w})) {
            if (inTextureMenu && m_leftClickedSprite_mat && m_materialEditorForSwapping) {
                m_leftClickedSprite_mat->m_material = mat;
                m_leftClickedSprite_mat = nullptr;
                m_openMaterialEditor = false;
            }
            else if (!inTextureMenu) {
                m_openMaterialEditor = true;
                m_materialEditorForSwapping = true;
                returnFlag = true;
            }  
        }
        if (ImGui::IsItemHovered()) {
            std::stringstream addr_ss;
            addr_ss << static_cast<const void*>(mat->m_diffuseTexture);
            std::string addr = std::string("TxtrAddr:") + addr_ss.str();
            ImGui::SetTooltip(addr.c_str());
        } 
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && inTextureMenu)
            m_rightClickedMaterial = mat;
        float col[4] { mat->m_colour.x, mat->m_colour.y, mat->m_colour.z, mat->m_colour.w };
        if (ImGui::ColorEdit4(("Color##" + id_s).c_str(), col)) {
            mat->m_colour = glm::vec4(col[0], col[1], col[2], col[3]);
        }
        if (ImGui::BeginMenu(("Blend Mode - " + mat->m_blending.asString() + "##" + id_s).c_str())) {
            if (ImGui::MenuItem("None")) {
                mat->m_blending.m_mode = BLEND_NONE;
            }
            if (ImGui::MenuItem("Add")) {
                mat->m_blending.m_mode = BLEND_ADD;
            }
            if (ImGui::MenuItem("Subtract")) {
                mat->m_blending.m_mode = BLEND_SUBTRACT;
            }
            if (ImGui::MenuItem("Reverse Subtract")) {
                mat->m_blending.m_mode = BLEND_REVERSE_SUBTRACT;
            }
            if (ImGui::MenuItem("Min")) {
                mat->m_blending.m_mode = BLEND_MIN;
            }
            if (ImGui::MenuItem("Max")) {
                mat->m_blending.m_mode = BLEND_MAX;
            }
            ImGui::EndMenu(); 
        }
        std::string filterTag = std::string("Texture Filtering - ") + ((mat->m_diffuseTexture->getFilterMode() == GL_NEAREST) ? "Nearest##" : "Linear##") + id_s;
        if (ImGui::BeginMenu(filterTag.c_str())) {
            if (ImGui::MenuItem("Nearest")) {
                mat->m_diffuseTexture->setFilterMode(GL_NEAREST);
            }
            if (ImGui::MenuItem("Linear")) {
                mat->m_diffuseTexture->setFilterMode(GL_LINEAR);
            }
            ImGui::EndMenu();
        }
    }

    ImGui::EndChild();
    return returnFlag;
}

void EditorUI::imgui() {
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Close")) {
                glfwSetWindowShouldClose(WindowManager::get()->m_wnd, true);
            }
            if (ImGui::MenuItem("Open")) {
                m_fileDiag.Open();
            }
            if (ImGui::MenuItem("Save")) {
                Project::get()->saveProject();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Project Settings & Preferences")) {
                m_settingsOpen = true; 
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (WindowManager::get()->getFullscreenMode() == FullscreenMode::Windowed || WindowManager::get()->getFullscreenMode() == FullscreenMode::Borderless) {
                if (ImGui::MenuItem("Go Fullscreen")) {
                    WindowManager::get()->setFullscreenMode(FullscreenMode::Fullscreen);
                }
            }
            else {
                if (ImGui::MenuItem("Go Windowed")) {
                    WindowManager::get()->setFullscreenMode(FullscreenMode::Windowed);
                    WindowManager::get()->centerWindow();
                }
            }
            if (ImGui::MenuItem("Center Window")) {
                WindowManager::get()->centerWindow();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu("Cameras")) {
                for (int i = 0; i < Camera::allCameras().size(); i++) {
                    std::string label = (Camera::allCameras()[i] == Camera::mainCamera) ? "Main Camera" : "Camera " + std::to_string(i);
                    if (ImGui::BeginMenu(label.c_str())){
                        if (ImGui::MenuItem("Reset View")) {
                            Camera::mainCamera->transform.setGlobalPosition({0,0,Camera::mainCamera->transform.getGlobalPosition().z});
                            Camera::mainCamera->m_zoomFactor = 10.0f;
                        }
                        ImGui::EndMenu();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    m_fileDiag.Display();

    ImGui::SetNextWindowPos(ImVec2(0,19));
    ImGui::SetNextWindowSize(ImVec2((WindowManager::get()->m_size.x / 100) * 20, WindowManager::get()->m_size.y));
    ImGui::Begin("Hello World!", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);

    if (ImGui::CollapsingHeader("Scene Tree", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginListBox("##scene_tree", {(WindowManager::get()->m_size.x / 100) * 19, (WindowManager::get()->m_size.y / 2) - 30})) {
            if (ImGui::TreeNodeEx(GameLevel::s_loadedLevel->m_levelName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf)) {
                //if (ImGui::IsItemClicked()) FLAG;
                std::vector<ScriptableEntity*> children = ScriptableEntity::s_sceneEntity->getChildren();
                for (int i = 0; i < children.size(); i++) {
                    if (!children[i]->m_hidden) {
                        drawEntityUI(children[i], 0);
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::Button((m_selectedSprites.size() > 1) ? "Delete Entities" : "Delete Entity")) {
                for (int i = 0; i < m_selectedSprites.size(); i++) {
                    ScriptableEntity* spr = m_selectedSprites[i];
                    Sprite* spr2 = dynamic_cast<Sprite*>(spr);
                    if (spr2) delete spr2;
                    else if (spr) delete spr;
                }
                m_selectedSprites.clear();
            }
            if (ImGui::Button("Create New Sprite")) {
                Sprite* spr = new Sprite;
                spr->create("Unnamed Sprite ");
                spr->setParent(ScriptableEntity::s_sceneEntity);
                std::cout << "Creating new blank (untextured) sprite" << std::endl;
            }
            if (m_selectedSprites.size() == 1) {
                if (ImGui::Button("Create New Child Sprite")) {
                    Sprite* spr = new Sprite;
                    spr->create("Unnamed Sprite ");
                    spr->setParent(m_selectedSprites[0]);
                    std::cout << "Creating new blank (untextured) sprite" << std::endl;
                }
            }
            ImGui::EndListBox();
        }
        
    }
    std::string tag;

    bool size0 = (m_selectedSprites.size() == 0);
    bool size1 = (m_selectedSprites.size() == 1);
    bool sizeGT1 = (m_selectedSprites.size() > 1);

    if (size0) tag = "Identity - None";
    if (size1) tag = "Identity - " + m_selectedSprites[0]->m_entityName;
    if (sizeGT1)  tag = "Identity - Multiple";

    ImGui::BeginChild("##Identity");
    if (ImGui::CollapsingHeader(tag.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        if (size1) {
            bool dirty = false;
            bool gDirty = false;
            ScriptableEntity* spr = m_selectedSprites[0];
            float t[3];
            float r[3];
            float s[3];
            float gt[3];
            float gr[3];
            float gs[3];
            
            glm::vec3 pos = spr->transform.getLocalPosition();
            glm::vec3 rot = spr->transform.getLocalRotation();
            glm::vec3 sca = spr->transform.getLocalScale();
            glm::vec3 gpos = spr->transform.getGlobalPosition();
            glm::vec3 grot = spr->transform.getGlobalRotation();
            glm::vec3 gsca = spr->transform.getGlobalScale();


            t[0] = pos.x;
            t[1] = pos.y;
            t[2] = pos.z;

            r[0] = rot.x;
            r[1] = rot.y;
            r[2] = rot.z;

            s[0] = sca.x;
            s[1] = sca.y;
            s[2] = sca.z;

            gt[0] = gpos.x;
            gt[1] = gpos.y;
            gt[2] = gpos.z;

            gr[0] = grot.x;
            gr[1] = grot.y;
            gr[2] = grot.z;

            gs[0] = gsca.x;
            gs[1] = gsca.y;
            gs[2] = gsca.z;

            float speed = ((glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)) ? 0.0001f : 0.1f;
            if ((glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)) speed = 1.0f;

            if (ImGui::CollapsingHeader("Transform [Local]", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::DragFloat3("Translate##l", t, speed)) {
                    if (ImGui::IsItemEdited()) {
                        dirty = true; 
                    }
                }
                if (ImGui::DragFloat3("Rotate##l", r, speed)) {
                    if (ImGui::IsItemEdited()) {
                        dirty = true; 
                    }
                }
                if (ImGui::DragFloat3("Scale##l", s, speed)) {
                    if (ImGui::IsItemEdited()) {
                        dirty = true; 
                    }
                }
            }

            if (ImGui::CollapsingHeader("Transform [Global]", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::DragFloat3("Translate##g", gt, speed)) {
                    if (ImGui::IsItemEdited()) {
                        gDirty = true; 
                    }
                }
                if (ImGui::DragFloat3("Rotate##g", gr, speed)) {
                    if (ImGui::IsItemEdited()) {
                        gDirty = true; 
                    }
                }
                if (ImGui::DragFloat3("Scale##g", gs, speed)) {
                    if (ImGui::IsItemEdited()) {
                        gDirty = true; 
                    }
                }
            }

            Sprite* spr_s = dynamic_cast<Sprite*>(spr);
            if (spr_s) {
                if (ImGui::CollapsingHeader("Material Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
                    if (spr_s->m_material->m_diffuseTexture != nullptr) {
                        if (ImGui::Button("Set to Texture Size")) {
                            spr_s->setScaleToTexelSize();
                        }
                        if (drawMaterialPreview(spr_s->m_material, ((WindowManager::get()->m_size.x / 100) * 15), false)) {
                            m_leftClickedSprite_mat = spr_s;
                        }
                    }
                    else {
                        if (ImGui::Button("Assign Material")) {
                            m_openMaterialEditor = true;
                            m_materialEditorForSwapping = true;
                            m_leftClickedSprite_mat = spr_s;
                        }
                    }
                }
            }

            if (dirty) {
                spr->transform.setLocalPosition({t[0], t[1], t[2]});
                spr->transform.setLocalRotation({r[0], r[1], r[2]});
                spr->transform.setLocalScale({s[0], s[1], s[2]});
            }
            if (gDirty) {
                spr->transform.setGlobalPosition({gt[0], gt[1], gt[2]});
                spr->transform.setGlobalRotation({gr[0], gr[1], gr[2]});
                spr->transform.setGlobalScale({gs[0], gs[1], gs[2]});
            }
        }
        else if (sizeGT1){
            ImGui::Text("I'm not doing the logic for that");
        }
    }
    ImGui::EndChild();
    ImGui::End(); 
    static bool textFlag = false;
    static char buf[32];
    ImGui::SetNextWindowPos({(WindowManager::get()->m_size.x / 100) * 20, (WindowManager::get()->m_size.y / 4) * 3});
    ImGui::SetNextWindowSize({WindowManager::get()->m_size.x - ((WindowManager::get()->m_size.x / 100) * 20), WindowManager::get()->m_size.y - ((WindowManager::get()->m_size.y / 4) * 3)});
    if (ImGui::Begin("Levels", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings)) {
        if (ImGui::Button("Create New")) {
            textFlag = true;
        }
        ImGui::SameLine();
        ImGui::Text("Levels");
        ImGui::NewLine();
        if (textFlag) {
            ImGui::InputText("##name", buf, 32);
            if (glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_ENTER) == GLFW_PRESS) {
                textFlag = false;
                GameLevel::createNewLevel(buf);
                for (int i = 0; i < 32; i++) {
                    buf[i] = {};
                }
            }
        }
        std::string _ = Project::get()->getLevelFolder();
        for (auto entry : std::filesystem::directory_iterator(_)) {
            if (File::getExtension(entry.path().string()) == LEV_DEFAULT_EXT) {
                std::string tag = entry.path().string();
                tag = tag.substr(tag.find_last_of('\\') + 1, tag.size() - 4);
                //std::cout << entry.path().string() << std::endl;
                if (ImGui::Button(tag.c_str())) {
                    std::string path = File::getFullPath(entry.path().string().c_str());
                    if (std::filesystem::path(path) != std::filesystem::path(GameLevel::s_loadedLevel->getFilePath())) {
                        for (int i = 0; i < m_selectedSprites.size(); i++) {
                            m_selectedSprites[i]->m_selected = false;
                        }
                        m_selectedSprites.clear();
                        GameLevel::loadLevel(path);
                    }
                } 
            }
        }
    }
    
    ImGui::End();

    if (m_settingsOpen) {
        if (ImGui::Begin("Project Settings & Preferences")) {
            ImGui::ColorEdit4("Clear Colour", Shader::m_clearCol);
            ImGui::ColorEdit4("Wireframe Colour", Shader::m_wireframeCol);
            ImGui::SliderFloat("Wireframe line width", &Shader::m_wireframeWidth, 0.3f, 2.0f);
            ImGui::ColorEdit4("Selection Colour", Shader::m_highlightCol);
            ImGui::ColorEdit4("Selection Box Colour", Shader::m_selectBoxCol);
            ImGui::SetCursorPos({
                ImGui::GetWindowSize().x - 55,
                ImGui::GetWindowSize().y - 30,
            });
            if (ImGui::Button("Close")) {
                m_settingsOpen = false; 
            }
            ImGui::End();
        }
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        m_contextOpen = true;
        m_contextOpenTF = true;

        if (!m_rcoSelectedInUI) {
            m_rightClickedObject = nullptr;
            for (int i = 0; i < Sprite::s_sprites.size(); i++) {
                Sprite* spr = dynamic_cast<Sprite*>(Sprite::s_sprites[i]); 
                if (spr->checkCollisionAgainstPoint(Mouse::get().screenPos()) && spr->m_visible && !spr->m_selected) {
                    m_rightClickedObject = spr;
                    break;
                }
            }
        }
        else {
            m_rcoSelectedInUI = false;
        }

    }

    if (m_contextOpen) {
        if (m_contextOpenTF) ImGui::SetNextWindowPos({Mouse::get().rawPos().x, Mouse::get().rawPos().y});
        if (ImGui::Begin("##Context Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
            
            ImGui::PushStyleColor(ImGuiCol_Button, {0,0,0,0});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0.3f});

            bool closeFlag = false;

            if (m_rightClickedMaterial) {
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y});
                std::string tag = std::string("Create Sprite w/ Material");
                if (ImGui::Button(tag.c_str())) {
                    Sprite* n = new Sprite;
                    n->create();
                    n->m_material = m_rightClickedMaterial;
                    n->setScaleToTexelSize();
                    m_rightClickedMaterial = nullptr;
                    closeFlag = true;
                }
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y});
                tag = std::string("Duplicate");
                if (ImGui::Button(tag.c_str())) {
                    Material* m = new Material;
                    m->m_colour = m_rightClickedMaterial->m_colour;
                    m->m_diffuseTexture = m_rightClickedMaterial->m_diffuseTexture;
                    m->m_shader = m_rightClickedMaterial->m_shader;
                    m->m_blending = m_rightClickedMaterial->m_blending;
                    closeFlag = true;
                }
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y});
                tag = std::string("Delete");
                if (ImGui::Button(tag.c_str())) {
                    m_rightClickedMaterial->destroy();
                }
                ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5});
            }

            if (m_rightClickedObject && m_selectedSprites.size() > 0) {
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y});
                std::string tag = std::string("Set " + m_rightClickedObject->m_entityName + " as parent");
                if (ImGui::Button(tag.c_str())) {
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        m_selectedSprites[i]->setParent(m_rightClickedObject);
                    }
                    closeFlag = true;
                }
                if (m_selectedSprites.size() == 1) {
                    ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y - 3});
                    tag = std::string("Add " + m_rightClickedObject->m_entityName + " as child");
                    if (ImGui::Button(tag.c_str())) {
                        m_rightClickedObject->setParent(m_selectedSprites[0]);
                        closeFlag = true;
                    }
                }
                ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5});
            }

            if (m_selectedSprites.size() == 1) {
                ScriptableEntity* ent = m_selectedSprites[0];

                if (ent->getParent() != ScriptableEntity::s_sceneEntity) {
                    ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y});
                    if (ImGui::Button("Unparent")) {
                        ent->setParent(ScriptableEntity::s_sceneEntity);
                    }
                }

                if (ImGui::BeginMenu("Create New Parent...")) {

                    if (ImGui::Button("Empty Entity")) {
                        ScriptableEntity* n = new ScriptableEntity;
                        n->create("Empty Parent ");
                        ScriptableEntity* p = ent->getParent();
                        if (p != nullptr) n->setParent(p);
                        ent->setParent(n);
                        closeFlag = true;
                    };

                    if (ImGui::Button("Sprite Entity")) {
                        Sprite* n = new Sprite;
                        n->create("Sprite Parent ");
                        n->setScaleToTexelSize();
                        ScriptableEntity* p = ent->getParent();
                        if (p != nullptr) n->setParent(p);
                        ent->setParent(n);
                        closeFlag = true;
                    };

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Create New Child...")) {
                    
                    if (ImGui::Button("Empty Entity")) {
                        ScriptableEntity* n = new ScriptableEntity;
                        n->create("Empty Child ");
                        ent->addChild(n);
                        closeFlag = true;
                    }

                    if (ImGui::Button("Sprite Entity")) {
                        Sprite* n = new Sprite;
                        n->create("Sprite Child ");
                        n->setScaleToTexelSize();
                        ent->addChild(n);
                        closeFlag = true;
                    }

                    ImGui::EndMenu();
                }
            }

            if (ImGui::BeginMenu("Create New...")) {
                
                if (ImGui::Button("Empty Entity")) {
                    ScriptableEntity* n = new ScriptableEntity;
                    n->create("Empty Entity ");
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        m_selectedSprites[i]->m_selected = false;
                    }
                    m_selectedSprites.clear();
                    n->m_selected = true;
                    m_selectedSprites.push_back(n);
                    closeFlag = true;
                }   

                if (ImGui::Button("Sprite Entity")) {
                    Sprite* n = new Sprite;
                    n->create("Sprite Entity ");
                    n->setScaleToTexelSize();
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        m_selectedSprites[i]->m_selected = false;
                    }
                    m_selectedSprites.clear();
                    n->m_selected = true;
                    m_selectedSprites.push_back(n);
                    closeFlag = true;
                }   

                ImGui::EndMenu();

            }

            if (m_selectedSprites.size() > 0) {
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y + 5});
                if (ImGui::Button((m_selectedSprites.size() > 1) ? "Delete Entities" : "Delete Entity")) {
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        ScriptableEntity* spr = m_selectedSprites[i];
                        Sprite* spr2 = dynamic_cast<Sprite*>(spr);
                        if (spr2) delete spr2;
                        else if (spr) delete spr;
                    }
                    m_selectedSprites.clear();
                    closeFlag = true;
                }
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y - 3});
                if (ImGui::Button("Deselect")) {
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        m_selectedSprites[i]->m_selected = false;
                    }
                    m_selectedSprites.clear();
                    closeFlag = true;
                }
            }
            
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) || closeFlag) {
                m_contextOpen = false;
            }
            ImGui::End();
        }
    }

    m_viewportSize = {WindowManager::get()->m_size.x - ((WindowManager::get()->m_size.x / 100) * 20), (((WindowManager::get()->m_size.y / 4) * 3) - 19)};
    Mouse::get().s_mouseOffset = {((WindowManager::get()->m_size.x / 100) * 20) / 2, ((((WindowManager::get()->m_size.y / 4) * 3) + 19) - WindowManager::get()->m_size.y) / 2};
    ImGui::SetNextWindowPos({(WindowManager::get()->m_size.x / 100) * 20, 19});
    ImGui::SetNextWindowSize(m_viewportSize);
    if (ImGui::Begin("Viewport", NULL, 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        
        m_toolWndHovering = !ImGui::IsWindowHovered();
        glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
        Camera::mainCamera->m_framebuffer->setSize({m_viewportSize.x, m_viewportSize.y});
        ImGui::SetCursorPos({0,0});
        ImGui::Image(Camera::mainCamera->m_framebuffer->m_renderTexture.getID(), m_viewportSize, {0, 1}, {1, 0});
        ImGui::SetCursorPos({10, 10});
        
        ImGui::PushStyleColor(ImGuiCol_Button, {1,1,1,0.1f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0.2f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1,1,1,0.3f});

        if (ImGui::ImageButton("##materialEditor", m_materialIcon.getID(), {64, 64}, {0, 1}, {1, 0})) {
            m_openMaterialEditor = !m_openMaterialEditor;
            m_materialEditorForSwapping = false;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Material Editor");

        ImGui::SetCursorPos({ImGui::GetCursorPos().x + 10, ImGui::GetWindowSize().y - 30});
        std::string zoom = "Zoom: " + std::to_string(Camera::mainCamera->m_zoomFactor / 10).substr(0, (Camera::mainCamera->m_zoomFactor >= 0.1) ? 5 : 10) + "x | ";
        ImGui::Text(zoom.c_str());
        ImGui::SameLine();
        ImGui::Value("X", Camera::mainCamera->transform.getGlobalPosition().x);
        ImGui::SameLine();
        ImGui::Value(" | Y", Camera::mainCamera->transform.getGlobalPosition().y);

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        
        m_viewportHasMouse = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();

    }
    ImGui::End();

    if (m_openMaterialEditor) {
        if (ImGui::Begin((m_materialEditorForSwapping) ? "Swap Material" : "Materials")) {
            if (ImGui::Button("Import")) {
                m_fileDiag.Open();
            }
            if (m_fileDiag.HasSelected()) {
                Material* m = new Material;
                m->m_diffuseTexture = Texture::createNewTextureFromPath(m_fileDiag.GetSelected().string().c_str());
                m_fileDiag.ClearSelected();
            }
            int j = 0;
            for (int i = 0; i < Material::s_allMaterials.size(); i++) {
                Material* mat = Material::s_allMaterials[i];
                if (mat->m_diffuseTexture) {
                    if (mat->m_diffuseTexture->isInitialised()) {
                        j++;
                        int modulo = 1;
                        int winSizex = ImGui::GetWindowSize().x;
                        if (winSizex > 100) {
                            int n = (int)(winSizex / 100) / 2;
                            for (int k = 0; k < n; k++) {
                                modulo += 1;
                            }
                        }

                        if ((j - 1) % modulo != 0) {
                            ImGui::SameLine();
                        }
                        
                        float sizex = (winSizex / modulo) - 10;
                        if (modulo == 1) ImGui::SetCursorPos({ImGui::GetCursorPos().x - 10, ImGui::GetCursorPos().y});

                        drawMaterialPreview(mat, sizex);
                    }
                } 
            }
        }
        ImGui::End();
    }
    glLineWidth(Shader::m_wireframeWidth * Camera::mainCamera->m_zoomFactor);
    glClearColor(Shader::m_clearCol[0], Shader::m_clearCol[1], Shader::m_clearCol[2], Shader::m_clearCol[3]);
    if (m_contextOpenTF) m_contextOpenTF = false;
}

void EditorUI::moveAlongHandleX() {
    m_axisHandle.m_xHandleSelected = true;
    for (int i = 0; i < m_selectedSprites.size(); i++) {
        float offset = m_selectedSprites[i]->transform.getGlobalPosition().x - m_axisHandle.transform.getLocalPosition().x;
        float mouseOffset  = m_mousePosWhenPressed.x - m_axisHandle.m_handlePosWhenPressed.x;
        m_selectedSprites[i]->transform.setGlobalPositionX(((Mouse::get().screenPos().x / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition().x));
        m_selectedSprites[i]->transform.setGlobalPosition((m_selectedSprites[i]->transform.getGlobalPosition() + glm::vec3(offset, 0, 0)) - glm::vec3(mouseOffset, 0, 0));
    }
}
void EditorUI::moveAlongHandleY() {
    m_axisHandle.m_yHandleSelected = true;
    for (int i = 0; i < m_selectedSprites.size(); i++) {
    float offset = m_selectedSprites[i]->transform.getGlobalPosition().y - m_axisHandle.transform.getLocalPosition().y;
        float mouseOffset  = m_mousePosWhenPressed.y - m_axisHandle.m_handlePosWhenPressed.y;
        m_selectedSprites[i]->transform.setGlobalPositionY(((Mouse::get().screenPos().y / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition().y));
        m_selectedSprites[i]->transform.setGlobalPosition((m_selectedSprites[i]->transform.getGlobalPosition() + glm::vec3(0, offset, 0)) - glm::vec3(0, mouseOffset, 0));
    }
}

void EditorUI::update() {

    GLFWwindow* m_wnd = WindowManager::get()->m_wnd;


    static bool sFlag = false;
    if (glfwGetKey(m_wnd, GLFW_KEY_R) == GLFW_PRESS && !sFlag) {
        sFlag = true;
        std::cout << "Playing Sound" << std::endl;
        AudioManager::get().playTestSound();
    }
    if (glfwGetKey(m_wnd, GLFW_KEY_R) == GLFW_RELEASE && sFlag) {
        sFlag = false;
    }
    


    // i need a proper input system
    static bool saveShortcutFlag = false;
    if (!saveShortcutFlag && (glfwGetKey(m_wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(m_wnd, GLFW_KEY_S) == GLFW_PRESS)) {
        saveShortcutFlag = true;
        Project::get()->saveProject();
    }
    if (saveShortcutFlag && (glfwGetKey(m_wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE && glfwGetKey(m_wnd, GLFW_KEY_S) == GLFW_RELEASE)) {
        saveShortcutFlag = false;
    }


    if (m_viewportHasMouse) {
        if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            Camera::mainCamera->transform.setGlobalPosition((glm::vec3(
                -Mouse::get().posDelta().x,
                Mouse::get().posDelta().y,
                0) / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition());
        }

        if (Mouse::get().scrollDelta().y != 0 && !m_toolWndHovering) {
                Camera::mainCamera->m_zoomFactor += (Mouse::get().scrollDelta().y / (10 / Camera::mainCamera->m_zoomFactor));
                Camera::mainCamera->m_zoomFactor = Math::clamp(Camera::mainCamera->m_zoomFactor, Camera::mainCamera->m_zoomFactorMin, Camera::mainCamera->m_zoomFactorMax);
                
                if (Camera::mainCamera->m_zoomFactor > Camera::mainCamera->m_zoomFactorMin && Camera::mainCamera->m_zoomFactor < Camera::mainCamera->m_zoomFactorMax) {
                    glm::vec2 mouseDistFromCenter = (Mouse::get().pos() - (WindowManager::get()->m_size / 2.0f)) / Camera::mainCamera->m_zoomFactor / 10.0f;

                    if (scroll_lf > Camera::mainCamera->m_zoomFactor) {
                        mouseDistFromCenter *= -1;
                    }

                    Camera::mainCamera->transform.setGlobalPosition((glm::vec3(
                    mouseDistFromCenter.x,
                    -mouseDistFromCenter.y,
                    0)) + Camera::mainCamera->transform.getGlobalPosition());
                }
                else {
                    Camera::mainCamera->m_zoomFactor = zoom_lf;
                }
                zoom_lf = Camera::mainCamera->m_zoomFactor;
        }
    }
    
    if (glfwGetKey(m_wnd, GLFW_KEY_DELETE) == GLFW_PRESS) {
        for (int i = 0; i < m_selectedSprites.size(); i++) {
            ScriptableEntity* spr = m_selectedSprites[i];
            Sprite* spr2 = dynamic_cast<Sprite*>(spr);
            if (spr2) delete spr2;
            else if (spr) delete spr;
        }
        m_selectedSprites.clear();
    }

    m_mousePressed = false;
    static bool hoverFlag = false;
    if (m_selectedSprites.size() != 0) {
        glm::vec3 pos{};
        int i;
        for (i = 0; i < m_selectedSprites.size(); i++) {
            pos += m_selectedSprites[i]->transform.getGlobalPosition();
        }
        pos = pos / (float)i;
        m_axisHandle.transform.setLocalPosition(pos);
        m_axisHandle.transform.setLocalScale(glm::vec3(200) / Camera::mainCamera->m_zoomFactor);

        m_axisHandle.tx.setLocalScale(glm::vec3(130, 30, 1) / Camera::mainCamera->m_zoomFactor);
        m_axisHandle.tx.setLocalPosition(m_axisHandle.transform.getLocalPosition() + m_axisHandle.transformx.getLocalPosition() + glm::vec3(m_axisHandle.tx.getLocalScale().x/2.0f, 0, 0));
        m_axisHandle.tx.setLocalRotation(m_axisHandle.transform.getLocalRotation() + m_axisHandle.transformx.getLocalRotation());

        m_axisHandle.ty.setLocalScale(glm::vec3(30, 130, 1) / Camera::mainCamera->m_zoomFactor);
        m_axisHandle.ty.setLocalPosition(m_axisHandle.transform.getLocalPosition() + m_axisHandle.transformy.getLocalPosition() + glm::vec3(0, m_axisHandle.ty.getLocalScale().y/2.0f, 0));
        m_axisHandle.ty.setLocalRotation(m_axisHandle.transform.getLocalRotation() + m_axisHandle.transformy.getLocalRotation());

        m_axisHandle.transformxy.setLocalScale(glm::vec3(20, 20, 1) / Camera::mainCamera->m_zoomFactor);
        m_axisHandle.transformxy.setLocalPosition(m_axisHandle.transform.getLocalPosition() + glm::vec3(30, 30, 0) / Camera::mainCamera->m_zoomFactor);

        if (!(m_axisHandle.m_xHandleSelected || m_axisHandle.m_yHandleSelected || m_axisHandle.m_xyHandleSelected)) {
            m_axisHandle.m_xHandle = m_axisHandle.tx.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
            m_axisHandle.m_yHandle = m_axisHandle.ty.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
            m_axisHandle.m_xyHandle = m_axisHandle.transformxy.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);

            if (m_axisHandle.m_xHandle && !m_multiSelect) {
                m_axisHandle.transformx.setLocalScaleY(1.1f); 
                hoverFlag = true;
            }
            else {
                m_axisHandle.transformx.setLocalScaleY(1);
            }

            if (!m_axisHandle.m_xHandle && m_axisHandle.m_yHandle && !m_multiSelect) {
                m_axisHandle.transformy.setLocalScaleY(1.1f); 
                hoverFlag = true;
            }
            else {
                m_axisHandle.transformy.setLocalScaleY(1);
            }

            if (m_axisHandle.m_xyHandle) {
                m_axisHandle.transformxy.setLocalScale(glm::vec3(1.1f) * glm::vec3(20, 20, 1) / Camera::mainCamera->m_zoomFactor); 
                hoverFlag = true;
            }
            else {
                m_axisHandle.transformxy.setLocalScale(glm::vec3(1) * glm::vec3(20, 20, 1) / Camera::mainCamera->m_zoomFactor);
            }
            if (!(m_axisHandle.m_xHandle || m_axisHandle.m_yHandle || m_axisHandle.m_xyHandle))
                hoverFlag = false;
        }

    }

    if (hoverFlag) {
        Mouse::get().setMouseCursor(Mouse::get().s_handCursor);
    }
    else
        Mouse::get().setMouseCursor(Mouse::get().s_defaultCursor);

    if (m_viewportHasMouse) {
        if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            m_mousePressed = true;

            if (!m_mousePressedLF) {
                m_mouseScreenPosWhenPressed = Mouse::get().screenPos();
                m_mousePosWhenPressed = (m_mouseScreenPosWhenPressed / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition();
                m_axisHandle.m_handlePosWhenPressed = m_axisHandle.transform.getLocalPosition();
            }

            if (m_axisHandle.m_xyHandle && !m_multiSelect) {
                moveAlongHandleX();
                moveAlongHandleY();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNESW);
            }
            else if (m_axisHandle.m_xHandle && !m_multiSelect) {
                moveAlongHandleX();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorWE);
            }
            else if (m_axisHandle.m_yHandle && !m_multiSelect) {
                moveAlongHandleY();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNS);
            }


            if (Mouse::get().screenPos() != m_mouseScreenPosWhenPressed && !(m_axisHandle.m_xHandle || m_axisHandle.m_yHandle || m_axisHandle.m_xyHandle)) {
                m_multiSelect = true;
            }
        }


        if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && m_mousePressedLF) {
            
            if (m_axisHandle.m_xHandleSelected || m_axisHandle.m_yHandleSelected || m_axisHandle.m_xyHandleSelected) {
                m_axisHandle.m_xHandleSelected = false;
                m_axisHandle.m_yHandleSelected = false;
                m_axisHandle.m_xyHandleSelected = false;
            }
            else {
                if (glfwGetKey(m_wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
                    m_selectedSprites.clear();
                    m_axisHandle.transform.setLocalScale({0,0,0});
                    for (int i = 0; i < GameLevel::s_loadedEntities.size(); i++) {
                        GameLevel::s_loadedEntities.at(i)->m_selected = false;
                    }
                }

                if (Mouse::get().screenPos() == m_mouseScreenPosWhenPressed || m_multiSelect) {
                    bool earlyBreak = false; // IGNORE
                    for (int i = 0; i < Sprite::s_sprites.size(); i++) {
                        if (!earlyBreak) {
                            bool condition = false;

                            Sprite* spr = dynamic_cast<Sprite*>(Sprite::s_sprites[i]); 
                            if (!m_multiSelect) condition = spr->checkCollisionAgainstPoint(Mouse::get().screenPos());
                            else                condition = spr->checkCollisionAgainstTransform(&m_selectionBox.transform);

                            if (condition && spr->m_visible && !spr->m_selected) {
                                spr->m_selected = true;
                                m_selectedSprites.push_back(spr);
                                if (!m_multiSelect) earlyBreak = true;
                            }
                        }
                    }
                }
                m_multiSelect = false;
            }
        }

    }
    scroll_lf = Camera::mainCamera->m_zoomFactor;
    m_mousePressedLF = m_mousePressed;
    imgui();
}

bool EditorUI::drawUI() {
    if (!ScriptableEntity::draw()) return false;
    BlendMode::useDefaultBlending();
    Camera::mainCamera->calculateVPMatrix();
    Camera::mainCamera->bindFramebuffer();
    if (m_multiSelect) {

        glm::vec3 m = (Mouse::get().screenPos() / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition();
        float size_x = m.x - m_mousePosWhenPressed.x;
        float size_y = m.y - m_mousePosWhenPressed.y;

        m_selectionBox.transform.setLocalScaleX(size_x);
        m_selectionBox.transform.setLocalScaleY(size_y);
        m_selectionBox.transform.setLocalPositionX(m_mousePosWhenPressed.x + size_x / 2);
        m_selectionBox.transform.setLocalPositionY(m_mousePosWhenPressed.y + size_y / 2);

        //std::cout << m_selectionBox.transform.getPosition().x << ", " << m_selectionBox.transform.getPosition().y << std::endl;

        if (m_selectionBox.m_material.m_shader == nullptr) m_selectionBox.m_material.m_shader = ShaderManager::get().s_shaderList[0];
        if (Camera::mainCamera != nullptr) {
            m_selectionBox.m_material.m_shader->setMat4(Camera::mainCamera->vpMatrix(), vpUniform);
        }
        else {
            std::cout << "WARNING : No camera exists in the scene!" << std::endl; 
        }
        m_selectionBox.transform.calculateWorldMatrix();
        m_selectionBox.m_material.m_shader->setMat4(m_selectionBox.transform.worldMatrix(), modelUniform);
        m_selectionBox.m_material.m_shader->use();
        m_selectionBox.m_mesh->bind();
        m_selectionBox.m_material.m_shader->setBool(true, "drawSelectionBox");
        m_selectionBox.m_material.m_shader->setVec4(glm::vec4(
            Shader::m_selectBoxCol[0],
            Shader::m_selectBoxCol[1],
            Shader::m_selectBoxCol[2],
            Shader::m_selectBoxCol[3]
        ), "selectionBoxCol");
        glDrawElements(m_selectionBox.m_mesh->m_topology, m_selectionBox.m_mesh->m_indexData.size(), GL_UNSIGNED_INT, 0); 
        m_selectionBox.m_material.m_shader->setBool(false, "drawSelectionBox");
    }
    if (m_selectedSprites.size() > 0) {

        if (Camera::mainCamera != nullptr) {
            m_axisHandle.m_material.m_shader->setMat4(Camera::mainCamera->vpMatrix(), vpUniform);
        }

        m_axisHandle.m_material.m_shader->setBool(true, "drawHandle");

        m_axisHandle.transformx.calculateWorldMatrix();
        m_axisHandle.transformy.calculateWorldMatrix();
        m_axisHandle.transformxy.calculateWorldMatrix();
        m_axisHandle.transform.calculateWorldMatrix();
        m_axisHandle.m_mesh.bind();

        //glDisable(GL_DEPTH_TEST);
        m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.transform.localMatrix() * m_axisHandle.transformx.localMatrix(), modelUniform);
        m_axisHandle.m_material.m_shader->setVec4({1, 0, 0, 1}, "handleCol");
        glDrawElements(m_axisHandle.m_mesh.m_topology, m_axisHandle.m_mesh.m_indexData.size(), GL_UNSIGNED_INT, 0); 

        m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.transform.localMatrix() * m_axisHandle.transformy.localMatrix(), modelUniform);
        m_axisHandle.m_material.m_shader->setVec4({0, 0, 1, 1}, "handleCol");
        glDrawElements(m_axisHandle.m_mesh.m_topology, m_axisHandle.m_mesh.m_indexData.size(), GL_UNSIGNED_INT, 0); 

        Mesh::Quad.bind();

        m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.transformxy.localMatrix(), modelUniform);
        m_axisHandle.m_material.m_shader->setVec4({0, 1, 0, 1}, "handleCol");
        glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0); 

        //glEnable(GL_DEPTH_TEST);
        m_axisHandle.m_material.m_shader->setBool(false, "drawHandle");
    }

    Framebuffer::useDefault();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return true;
}

void EditorUI::create(std::string name) {
    ScriptableEntity::create(name);
}

EditorUI::EditorUI() {
    create();
}

EditorUI::~EditorUI() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}

void EditorUI::createAxisHandle() {
    
    float width = 0.015f;
    float height = 0.5f;
    float arrowHeight = 0.1f;
    float yOffset = 0.1f;

    std::vector<Vertex> v {
        {{-width, yOffset, 0}},
        {{ width, yOffset, 0}},
        {{ width, height - arrowHeight + yOffset, 0}},
        {{-width, height - arrowHeight + yOffset, 0}},

        {{-width*2, height - arrowHeight + yOffset, 0}},
        {{ width*2, height - arrowHeight + yOffset, 0}},
        {{ 0, height + yOffset, 0}}
    };

    std::vector<unsigned int> ind {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6
    };

    m_axisHandle.m_material.m_doSerialize = false;
    m_axisHandle.m_mesh.setupMesh(v, ind);
    m_axisHandle.m_material.m_shader = ShaderManager::get().s_shaderList[0];
    m_axisHandle.transformx.setLocalRotationZ(90);
}