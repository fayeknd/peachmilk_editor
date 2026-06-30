
#include "editor.hpp"
#include "../windowmanager/WindowManager.hpp"
#include "../render/camera.hpp"
#include "../system/mouse.hpp"
#include "../shader/shader.hpp"
#include "../system/file.hpp"
#include "../audio/audiosource.hpp"
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include "../render/text/text.hpp"

/*
-------------------------------------------------------------------------
DO. NOT. LOOK AT THIS FILE.
IT'S OKAY. IT WORKS. THAT'S ALL YOU NEED TO KNOW.
DON'T SCROLL PLEASE.
PLEASE.

this is somethign that i didnt think would need so much code and grew
massively and now is an unoptimised and messy monstrosity so trust me
do not look at this its best if you dont














-------------------------------------------------------------------------
*/

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
    for (i = 0;  i < count;  i++) {
        if (!File::fileExists(paths[i])) {
            std::cout << "Provided file " << paths[i] << " is invalid";
        }
        else {
            std::string ext = File::getExtension(paths[i]);
            if (ext == "ttf" || ext == "otf") {
                TextFont* font = TextFont::loadFont(paths[i], -1);
                if (font) {
                    TextRenderer* text = new TextRenderer;
                    text->create(std::filesystem::path(paths[i]).filename().string());
                    text->setFont(font);
                    text->setText(std::filesystem::path(paths[i]).filename().string());
                }
            }
            for (int j = 0; j < EditorUI::m_recognisedImageFormatExts.size(); j++) {
                if (ext == EditorUI::m_recognisedImageFormatExts[j]) {
                    Sprite* spr = new Sprite;
                    spr->create();
                    spr->m_material = new Material;
                    spr->m_material->m_diffuseTexture = Texture::createNewTextureFromPath(paths[i], GL_NEAREST);
                    EditorUI::m_entityJustDropped = spr;
                    spr->setScaleToTexelSize();
                }
            }
        }
    }
}

bool EditorUI::IsItemDroppedHere() {
    if (ImGui::IsItemHovered() && m_draggedItem != m_draggedItemLF && m_draggedItem == "") return true;
    return false;
}

void EditorUI::_start() {

    m_serialize = false;
    m_selectionBox.m_serialize = false;
    m_selectionBox.m_destroyOnLoad = false;
    m_selectionBox.m_material.m_serialize = false;
    glm::vec2 size = WindowManager::get()->m_size;
    Camera::mainCamera->m_framebuffer = new Framebuffer;
    Camera::mainCamera->m_framebuffer->create(size.x, size.y);
    Camera::mainCamera->m_serialize = false;
    Camera::mainCamera->m_destroyOnLoad = false;

    m_materialIcon = *Texture::createNewTextureFromPath("editor_data/ui/materialIcon.png", GL_LINEAR, GL_RGBA, GL_TEXTURE_2D, false);
    m_channelMixerIcon = *Texture::createNewTextureFromPath("editor_data/ui/channelMixerIcon.png", GL_LINEAR, GL_RGBA, GL_TEXTURE_2D, false);
    m_folderIcon = *Texture::createNewTextureFromPath("editor_data/ui/folderIcon_pink.png", GL_LINEAR, GL_RGBA, GL_TEXTURE_2D, false);
    m_unrecognisedFileIcon = *Texture::createNewTextureFromPath("editor_data/ui/unrecognisedFileIcon.png", GL_LINEAR, GL_RGBA, GL_TEXTURE_2D, false);

    m_fileDiag.SetTitle("File Browser");
    m_fileDiag.SetDirectory(File::getWorkingDirectory());

    glfwSetDropCallback(WindowManager::get()->m_wnd, drop_callback);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(WindowManager::get()->m_wnd, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    createAxisHandle();

    setImGuiStyle(7,7,7);

#define _THISBIT
#ifdef THISBIT
    TextRenderer* text = new TextRenderer;
    text->create("Test TextRenderer");
    TextFont* newFont = new TextFont;
    newFont->loadTypeFace("Angeles-Regular.ttf", 256);
    text->m_font = newFont;
#endif
    m_hidden = true;
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

}

void EditorUI::setImGuiStyle(int hue07, int alt07, int nav07, int lit01, int compact01, int border01, int shape0123) {
    ImGuiStyle& style = ImGui::GetStyle();

    const float _8 = compact01 ? 4 : 8;
    const float _4 = compact01 ? 2 : 4;
    const float _2 = compact01 ? 0.5 : 1;

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.3f;

    style.WindowPadding = ImVec2(4, _8);
    style.FramePadding = ImVec2(4, _4);
    style.ItemSpacing = ImVec2(_8, _2 + _2);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.IndentSpacing = 16;
    style.ScrollbarSize = compact01 ? 12 : 18;
    style.GrabMinSize = compact01 ? 16 : 20;

    style.WindowBorderSize = border01;
    style.ChildBorderSize = border01;
    style.PopupBorderSize = border01;
    style.FrameBorderSize = 0;

    style.WindowRounding = 4;
    style.ChildRounding = 6;
    style.FrameRounding = shape0123 == 0 ? 0 : shape0123 == 1 ? 4 : 12;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 30 * 8 + 4;
    style.GrabRounding = style.FrameRounding;

    style.TabBorderSize = 0;
    style.TabBarBorderSize = 2;
    style.TabBarOverlineSize = 2;
    style.TabCloseButtonMinWidthSelected = -1; // -1:always visible, 0:visible when hovered, >0:visible when hovered if minimum width
    style.TabCloseButtonMinWidthUnselected = -1;
    style.TabRounding = 30;

    style.CellPadding = ImVec2(8.0f, 4.0f);

    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Right;

    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.5f, 0.5f);
    style.SeparatorTextAlign.x = 1.00f;
    style.SeparatorTextBorderSize = 1;
    style.SeparatorTextPadding = ImVec2(0,0);

    style.WindowMinSize = ImVec2(32.0f, 16.0f);
    style.ColumnsMinSpacing = 6.0f;

    // diamond sliders
    style.CircleTessellationMaxError = shape0123 == 3 ? 4.00f : 0.30f;

    auto lit = [&](ImVec4 hi) {
        float h,s,v; ImGui::ColorConvertRGBtoHSV(hi.x,hi.y,hi.z, h,s,v);
        ImVec4 lit = ImColor::HSV(h,s*0.80,v*1.00, hi.w).Value;
        return lit;
    };
    auto dim = [&](ImVec4 hi) {
        float h,s,v; ImGui::ColorConvertRGBtoHSV(hi.x,hi.y,hi.z, h,s,v);
        ImVec4 dim = ImColor::HSV(h,s,lit01 ? v*0.65:v*0.65, hi.w).Value;
        if( hi.z > hi.x && hi.z > hi.y ) return ImVec4(dim.x,dim.y,hi.z,dim.w);
        return dim;
    };

    const ImVec4 cyan    = ImVec4(000/255.f, 192/255.f, 255/255.f, 1.00f);
    const ImVec4 red     = ImVec4(230/255.f, 000/255.f, 000/255.f, 1.00f);
    const ImVec4 yellow  = ImVec4(240/255.f, 210/255.f, 000/255.f, 1.00f);
    const ImVec4 orange  = ImVec4(255/255.f, 144/255.f, 000/255.f, 1.00f);
    const ImVec4 lime    = ImVec4(192/255.f, 255/255.f, 000/255.f, 1.00f);
    const ImVec4 aqua    = ImVec4(000/255.f, 255/255.f, 192/255.f, 1.00f);
    const ImVec4 magenta = ImVec4(255/255.f, 000/255.f,  88/255.f, 1.00f);
    const ImVec4 purple  = ImVec4(192/255.f, 000/255.f, 255/255.f, 1.00f);

    ImVec4 alt = cyan;
    if( alt07 == 0 || alt07 == 'C' ) alt = cyan;
    else if( alt07 == 1 || alt07 == 'R' ) alt = red;
    else if( alt07 == 2 || alt07 == 'Y' ) alt = yellow;
    else if( alt07 == 3 || alt07 == 'O' ) alt = orange;
    else if( alt07 == 4 || alt07 == 'L' ) alt = lime;
    else if( alt07 == 5 || alt07 == 'A' ) alt = aqua;
    else if( alt07 == 6 || alt07 == 'M' ) alt = magenta;
    else if( alt07 == 7 || alt07 == 'P' ) alt = purple;
    if( lit01 ) alt = dim(alt);

    ImVec4 hi = cyan, lo = dim(cyan);
    if( hue07 == 0 || hue07 == 'C' ) lo = dim( hi = cyan );
    else if( hue07 == 1 || hue07 == 'R' ) lo = dim( hi = red );
    else if( hue07 == 2 || hue07 == 'Y' ) lo = dim( hi = yellow );
    else if( hue07 == 3 || hue07 == 'O' ) lo = dim( hi = orange );
    else if( hue07 == 4 || hue07 == 'L' ) lo = dim( hi = lime );
    else if( hue07 == 5 || hue07 == 'A' ) lo = dim( hi = aqua );
    else if( hue07 == 6 || hue07 == 'M' ) lo = dim( hi = magenta );
    else if( hue07 == 7 || hue07 == 'P' ) lo = dim( hi = purple );
//    if( lit01 ) { ImVec4 tmp = hi; hi = lo; lo = lit(tmp); }

    ImVec4 nav = orange;
    if( nav07 == 0 || nav07 == 'C' ) nav = cyan;
    else if( nav07 == 1 || nav07 == 'R' ) nav = red;
    else if( nav07 == 2 || nav07 == 'Y' ) nav = yellow;
    else if( nav07 == 3 || nav07 == 'O' ) nav = orange;
    else if( nav07 == 4 || nav07 == 'L' ) nav = lime;
    else if( nav07 == 5 || nav07 == 'A' ) nav = aqua;
    else if( nav07 == 6 || nav07 == 'M' ) nav = magenta;
    else if( nav07 == 7 || nav07 == 'P' ) nav = purple;
    if( lit01 ) nav = dim(nav);

    const ImVec4
    link  = ImVec4(0.26f, 0.59f, 0.98f, 1.00f),
    grey0 = ImVec4(0.04f, 0.05f, 0.07f, 1.00f),
    grey1 = ImVec4(0.08f, 0.09f, 0.11f, 1.00f),
    grey2 = ImVec4(0.10f, 0.11f, 0.13f, 1.00f),
    grey3 = ImVec4(0.12f, 0.13f, 0.15f, 1.00f),
    grey4 = ImVec4(0.16f, 0.17f, 0.19f, 1.00f),
    grey5 = ImVec4(0.18f, 0.19f, 0.21f, 1.00f);

    #define Luma(v,a) ImVec4((v)/100.f,(v)/100.f,(v)/100.f,(a)/100.f)

    style.Colors[ImGuiCol_Text]                      = Luma(100,100);
    style.Colors[ImGuiCol_TextDisabled]              = Luma(39,100);
    style.Colors[ImGuiCol_WindowBg]                  = grey1;
    style.Colors[ImGuiCol_ChildBg]                   = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]                   = grey1;
    style.Colors[ImGuiCol_Border]                    = grey4;
    style.Colors[ImGuiCol_BorderShadow]              = grey1;
    style.Colors[ImGuiCol_FrameBg]                   = ImVec4(0.11f, 0.13f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]            = grey4;
    style.Colors[ImGuiCol_FrameBgActive]             = grey4;
    style.Colors[ImGuiCol_TitleBg]                   = grey0;
    style.Colors[ImGuiCol_TitleBgActive]             = grey0;
    style.Colors[ImGuiCol_TitleBgCollapsed]          = grey1;
    style.Colors[ImGuiCol_MenuBarBg]                 = grey2;
    style.Colors[ImGuiCol_ScrollbarBg]               = grey0;
    style.Colors[ImGuiCol_ScrollbarGrab]             = grey3;
    style.Colors[ImGuiCol_ScrollbarGrabHovered]      = lo;
    style.Colors[ImGuiCol_ScrollbarGrabActive]       = hi;
    style.Colors[ImGuiCol_CheckMark]                 = alt;
    style.Colors[ImGuiCol_SliderGrab]                = lo;
    style.Colors[ImGuiCol_SliderGrabActive]          = hi;
    style.Colors[ImGuiCol_Button]                    = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]             = lo;
    style.Colors[ImGuiCol_ButtonActive]              = grey5;
    style.Colors[ImGuiCol_Header]                    = grey3;
    style.Colors[ImGuiCol_HeaderHovered]             = lo;
    style.Colors[ImGuiCol_HeaderActive]              = hi;
    style.Colors[ImGuiCol_Separator]                 = ImVec4(0.13f, 0.15f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]          = lo;
    style.Colors[ImGuiCol_SeparatorActive]           = hi;
    style.Colors[ImGuiCol_ResizeGrip]                = Luma(15,100);
    style.Colors[ImGuiCol_ResizeGripHovered]         = lo;
    style.Colors[ImGuiCol_ResizeGripActive]          = hi;
    style.Colors[ImGuiCol_InputTextCursor]           = Luma(100,100);
    style.Colors[ImGuiCol_TabHovered]                = grey3;
    style.Colors[ImGuiCol_Tab]                       = grey1;
    style.Colors[ImGuiCol_TabSelected]               = grey3;
    style.Colors[ImGuiCol_TabSelectedOverline]       = hi;
    style.Colors[ImGuiCol_TabDimmed]                 = grey1;
    style.Colors[ImGuiCol_TabDimmedSelected]         = grey1;
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = lo;
    style.Colors[ImGuiCol_PlotLines]                 = grey5;
    style.Colors[ImGuiCol_PlotLinesHovered]          = lo;
    style.Colors[ImGuiCol_PlotHistogram]             = grey5;
    style.Colors[ImGuiCol_PlotHistogramHovered]      = lo;
    style.Colors[ImGuiCol_TableHeaderBg]             = grey0;
    style.Colors[ImGuiCol_TableBorderStrong]         = grey0;
    style.Colors[ImGuiCol_TableBorderLight]          = grey0;
    style.Colors[ImGuiCol_TableRowBg]                = grey3;
    style.Colors[ImGuiCol_TableRowBgAlt]             = grey2;
    style.Colors[ImGuiCol_TextLink]                  = link;
    style.Colors[ImGuiCol_TextSelectedBg]            = Luma(39,100);
    style.Colors[ImGuiCol_TreeLines]                 = Luma(39,100);
    style.Colors[ImGuiCol_DragDropTarget]            = nav;
    style.Colors[ImGuiCol_NavCursor]                 = nav;
    style.Colors[ImGuiCol_NavWindowingHighlight]     = lo;
    style.Colors[ImGuiCol_NavWindowingDimBg]         = Luma(0,63);
    style.Colors[ImGuiCol_ModalWindowDimBg]          = Luma(0,63);


    #undef Luma
}

void EditorUI::drawProbablyComplicatedFileBrowserTypeShi() {\
    static bool textFlag = false;
    static char buf[32];
    ImGui::SetNextWindowPos({(WindowManager::get()->m_size.x / 100) * 20, (WindowManager::get()->m_size.y / 4) * 3});
    ImVec2 winSize = {WindowManager::get()->m_size.x - ((WindowManager::get()->m_size.x / 100) * 20), WindowManager::get()->m_size.y - ((WindowManager::get()->m_size.y / 4) * 3)};
    ImGui::SetNextWindowSize(winSize);
    if (ImGui::Begin("EngineFileManager", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::CollapsingHeader("File Browser", ImGuiTreeNodeFlags_Leaf);
        if (m_fbCurrentDir == "") m_fbCurrentDir = Project::get()->getGameDataFolder();
        std::filesystem::path entry(m_fbCurrentDir);
        std::vector<std::string> paths;
        while (entry.has_parent_path()) {
            if (entry == entry.parent_path()) {
                paths.insert(paths.begin(), entry.string().substr(0, 3));
                break;
            }
            std::string dir = entry.string();
            entry = entry.parent_path();
            dir = dir.substr(dir.find_last_of("\\") + 1, dir.size());
            if (dir != "") paths.insert(paths.begin(), dir);
        }
        if (ImGui::Button("Quick: Project##__pdir")) {
            m_fbCurrentDir = Project::get()->getGameDataFolder();
        }
        ImGui::SameLine();
        if (ImGui::Button("Quick: Levels##__pdir")) {
            m_fbCurrentDir = Project::get()->getLevelFolder();
        }
        for (int i = 0; i < paths.size(); i++) {
            ImGui::SameLine();
            if (ImGui::Button(paths[i].c_str())) {
                std::string newDir;
                for (int j = 0; j < i + 1; j++) {
                    if (j != i)
                        newDir += paths[j] + "\\";
                    else
                        newDir += paths[j];
                }
                m_fbCurrentDir = newDir;
                break;
            }
        }

        int i = 0;
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::SliderInt("Icon Size##imgui", &m_iconSize, 3, 30)) {
            if (m_iconsPerRow > m_iconSize) m_iconsPerRow = m_iconSize;
        };

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::SliderInt("Icons Per Row##imgui", &m_iconsPerRow, 3, m_iconSize);

        ImGui::SameLine();
        ImGui::Checkbox("Hidden?##imgui", &m_showHiddenFiles);

        ImGui::SameLine();
        ImGui::Checkbox("Just folders?##imgui", &m_showOnlyFolders);

        ImVec2 btnSize = {winSize.x / m_iconSize, winSize.x / m_iconSize};
        for (auto entry : std::filesystem::directory_iterator(m_fbCurrentDir)) {
            std::string dir = entry.path().string();
            dir = dir.substr(dir.find_last_of("\\") + 1, dir.size());
            if ((dir.substr(0, 1) != "." || m_showHiddenFiles) && (entry.is_directory() || !m_showOnlyFolders)) {
                static ImVec2 cursorPos;
                static float offset;
                if (i == 0) {
                    offset = ((winSize.x - btnSize.x) / m_iconsPerRow);
                    cursorPos = {0, ImGui::GetCursorPosY()};
                }
                else {
                    if (i % (m_iconsPerRow + 1) == 0 && i != 0) {
                        cursorPos = {0, cursorPos.y + btnSize.y + 20};
                    }
                    else {
                        cursorPos = {cursorPos.x + offset, cursorPos.y};
                    }
                }
                ImGui::SetCursorPos(cursorPos);
                std::string tag = std::string(entry.path().filename().string() + std::string(" ##Dir")).c_str();
                ImGui::ImageButton(tag.c_str(), (entry.is_directory()) ? m_folderIcon.getID() : m_unrecognisedFileIcon.getID(), btnSize, {0, 1}, {1, 0}, {0,0,0,0});
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    std::string ext = File::getExtension(entry.path().filename().string());
                    if (entry.is_directory()) {
                        try {
                            // check if the directory is accessible
                            bool hasItem = false;
                            for (auto entry2 : std::filesystem::directory_iterator(entry)) {
                                m_fbCurrentDir = entry.path().string();
                                hasItem = true;
                                break;
                            }
                            if (!hasItem) m_fbCurrentDir = entry.path().string();
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            createPopup(e.what(), "Read Error");
                        }
                    }
                    else {
                        bool flag = false;
                        for (int x = 0; x < m_recognisedImageFormatExts.size(); x++) {
                            std::string p = entry.path().string();
                            if (ext == m_recognisedImageFormatExts[x]) {
                                const char* paths[1];
                                paths[0] = p.c_str();
                                std::cout << paths [0] << std::endl;
                                drop_callback(WindowManager::get()->m_wnd, 1, paths);
                                flag = true;
                                break;
                            }
                        }
                        if (!flag) {
                            if (ext == LEV_DEFAULT_EXT) {
                                GameLevel::loadLevel(entry.path().string());
                            }
                            else if (ext == "ttf" || ext == "otf") {
                                TextFont* font = TextFont::loadFont(entry.path().string(), -1);
                                if (font) {
                                    TextRenderer* text = new TextRenderer;
                                    text->create(entry.path().filename().string());
                                    text->setFont(font);
                                    text->setText(entry.path().filename().string());
                                }
                            }
                            else {
                                createPopup("Filetype has no action.", "Incompatiable Type");
                            }
                        }
                    }
                }
                if (ImGui::IsItemHovered()) {
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        if (m_mouseDownFF) {
                            m_draggedItem = entry.path();
                        }
                        m_mouseDownFF = false;
                    }
                }
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    m_draggedItem = "";
                    if (!m_mouseDownFF) m_mouseDownFF = true;
                }
                if (IsItemDroppedHere() && m_draggedItemLF != entry.path() && entry.is_directory()){
                    try {
                        if (std::filesystem::directory_entry(m_draggedItemLF).is_directory()) {
                            std::string name = std::filesystem::path(m_draggedItemLF).filename().string();
                            std::filesystem::rename(m_draggedItemLF, entry.path().string() + "\\" + name);
                        }
                        else {
                            std::filesystem::copy(std::filesystem::path(m_draggedItemLF), entry);
                            remove(m_draggedItemLF.string().c_str());
                        }
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        createPopup(e.what(), "Read Error");
                    }
                }

                ImGui::SetCursorPos(cursorPos);
                ImGui::Text(entry.path().filename().string().c_str());

                if (m_draggedItem == entry.path()) {
                    ImGui::Begin("##__dragged_item", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoSavedSettings);

                    ImGui::SetWindowPos({Mouse::get().rawPos().x, Mouse::get().rawPos().y});
                    ImGui::SetWindowPos({ImGui::GetWindowPos().x - (ImGui::GetWindowWidth() / 2), ImGui::GetWindowPos().y - (ImGui::GetWindowHeight() / 2)});

                    ImGui::Image((entry.is_directory()) ? m_folderIcon.getID() : m_unrecognisedFileIcon.getID(), btnSize, {0, 1}, {1, 0});
                    ImGui::SetCursorPos({0,0});
                    ImGui::Text(entry.path().filename().string().c_str());
                    ImGui::End();
                }

                i++;
            }
        }
    }

    ImGui::End();
}

float scroll_lf;
float zoom_lf;
bool test = true;

void EditorUI::createPopup(std::string text, std::string title) {
    m_popupText = text;
    m_createPopup = true;
    m_createPopupFF = true;
    m_popupTitle = title;
}

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
            if (spr != m_selectedSprites[0]) {
                m_rightClickedObject = spr;
                m_rcoSelectedInUI = true;
            }
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
                m_leftClickedSprite_mat->setScaleToTexelSize();
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
    drawProbablyComplicatedFileBrowserTypeShi();
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
                        if (ImGui::MenuItem(Camera::allCameras()[i]->m_isOrtho ? "Perspective View" : "Orthographic View")) {
                            Camera::allCameras()[i]->m_isOrtho = !Camera::allCameras()[i]->m_isOrtho;
                        }
                        if (ImGui::MenuItem("Reset View")) {
                            Camera::allCameras()[i]->transform.setGlobalPosition({0,0,Camera::mainCamera->transform.getGlobalPosition().z});
                            Camera::allCameras()[i]->m_zoomFactor = 10.0f;
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
                    GameLevel::deleteEntity(m_selectedSprites[i]);
                    m_selectedSprites.erase(m_selectedSprites.begin() + i);
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
            ImGui::Text((std::string("EntityID: ") + std::to_string(spr->getID())).c_str());
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

            ImGui::NewLine();

            bool plock, slock, rlock;
            plock = spr->transform.isPosRounded();
            slock = spr->transform.isScaleRounded();
            rlock = spr->transform.isRotRounded();
            if (ImGui::Checkbox("Lock pos to int?", &plock)) {
                spr->transform.setPosRounded(plock);
            }
            if (ImGui::Checkbox("Lock scale to int?", &slock)) {
                spr->transform.setScaleRounded(slock);
            }
            if (ImGui::Checkbox("Lock rotation to int?", &rlock)) {
                spr->transform.setRotRounded(rlock);
            }

            ImGui::Checkbox("Destroy on Load?", &spr->m_destroyOnLoad);
            ImGui::Checkbox("Serialize?", &spr->m_serialize);
            ImGui::NewLine();

            Sprite* spr_s = dynamic_cast<Sprite*>(spr);
            #pragma region SpriteIdentity
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
            #pragma endregion
            AudioSource* audio = dynamic_cast<AudioSource*>(spr);
            TextRenderer* text = dynamic_cast<TextRenderer*>(spr);
            #pragma region TextIdentity
            if (text) {

                if (ImGui::CollapsingHeader("TextRenderer Properties", ImGuiTreeNodeFlags_DefaultOpen)) {

                    std::string fnt = "Loaded font: " + ((text->getFont()) ? std::filesystem::path(text->getFont()->getFontLoc()).filename().string() : "None");
                    ImGui::Text(fnt.c_str());
                    if (ImGui::Button("Set Font...")) {
                        m_fileDiag.SetDirectory(Project::get()->getFontFolder());
                        m_fileDiag.Open();
                    }
                    if (m_fileDiag.HasSelected()) {
                        std::filesystem::path fontFile = m_fileDiag.GetSelected();
                        if (!(File::getExtension(fontFile.string()) == "ttf" || File::getExtension(fontFile.string()) == "otf")) {
                            std::cout << File::getExtension(fontFile.string()) << std::endl;
                            createPopup("Font file not supported!", "Unsupported Font File");
                            m_fileDiag.ClearSelected();
                        }
                        else {
                            ImGui::Text("Choose font size, click Load.");
                            ImGui::Text("Leave at 0 to find loaded font.");
                            ImGui::DragInt("Font Size", &m_fontSize, 1, 0, 256);
                            if (ImGui::Button("Load")) {
                                m_fileDiag.ClearSelected();
                                TextFont* f = TextFont::loadFont(fontFile.string(), (m_fontSize != 0) ? m_fontSize : -1);
                                if (f) text->setFont(f);
                                m_fontSize = ENGINE_DEFTAULT_FONT_SIZE;
                            }
                        }
                    }
                    if (text->getFont()) {
                        float spacing = text->getFontLineSpacing();
                        if (ImGui::InputFloat("Line Spacing", &spacing)) {
                            text->setFontLineSpacing(spacing);
                        }
                    }
                    ImGui::NewLine();
                    static char tReplacementText[1024];
                    memset(&tReplacementText, 0, 1024);
                    for (int i = 0; i < std::min(static_cast<int>(text->getText().size()), 1024); i++) {
                        tReplacementText[i] = text->getText()[i];
                    }
                    std::string tag = "##textfield" + std::to_string(text->getID());
                    if (ImGui::InputText(tag.c_str(), tReplacementText, 1024)) {
                        text->setText(tReplacementText);
                    }
                    bool copy = text->getCopyToChildren();
                    if (ImGui::Checkbox("Copy text to children?", &copy)) {
                        text->setCopyToChildren(copy);
                    }
                    std::string alignment;
                    switch (text->getTextAlignment()) {
                        case TextAlignment::Left:
                        alignment = "Left";
                        break;
                        case TextAlignment::Center:
                        alignment = "Center";
                        break;
                        case TextAlignment::Right:
                        alignment = "Right";
                        break;
                        default:
                        alignment = "Unknown";
                    }
                    alignment = "Text Alignment (" + alignment + ")";
                    if (ImGui::BeginMenu(alignment.c_str())) {
                        if (ImGui::MenuItem("Left")) {
                            text->setTextAlignment(TextAlignment::Left);
                        }
                        if (ImGui::MenuItem("Center")) {
                            text->setTextAlignment(TextAlignment::Center);
                        }
                        if (ImGui::MenuItem("Right")) {
                            text->setTextAlignment(TextAlignment::Right);
                        }
                        ImGui::EndMenu();
                    }
                }
                float c[4] {text->m_colourMultiplier.x, text->m_colourMultiplier.y, text->m_colourMultiplier.z, text->m_colourMultiplier.w};
                if (ImGui::ColorEdit4("Text Colour", c)) {
                    text->m_colourMultiplier = glm::vec4(c[0], c[1], c[2], c[3]);
                }
            }
            #pragma endregion
            #pragma region AudioIdentity
            if (audio) {
                if (ImGui::CollapsingHeader("AudioSource Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
                    float v = audio->getVolume() * 100;
                    float p = audio->getPitch() - 1;
                    float d = audio->getDoppler();
                    float l = audio->get3DLevel();
                    float f[2] = {audio->getMinFalloff(), audio->getMaxFalloff()};

                    std::string cgtag("Channel Group (" + ((ChannelGroup::getChannelGroup(audio->getChannelGroupName())) ? audio->getChannelGroupName() : "None") + ")");


                    if (ImGui::BeginMenu(cgtag.c_str())) {
                        for (int i = 0; i < ChannelGroup::s_channelGroups.size(); i++) {
                            ChannelGroup * cg = ChannelGroup::s_channelGroups[i];
                            if (ImGui::MenuItem(cg->getName().c_str())) {
                                audio->setChannelGroup(cg->getName());
                            }
                        }
                        if (ImGui::MenuItem("None")) {
                            audio->setChannelGroup(nullptr);
                        }
                        ImGui::EndMenu();
                    }

                    if (ImGui::SliderFloat("Volume", &v, 0, 300, "%.0f%%")) {
                        audio->setVolume(v / 100);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Channel volume (Between 0 and 300, can be set. programmatically higher.)");
                    }

                    if (ImGui::SliderFloat("Pitch", &p, -1, 1, "%.3f")) {
                        audio->setPitch(p + 1);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Channel Pitch/Frequency. (Between 0 and 1.)");
                    }

                    if (ImGui::SliderFloat("Doppler", &d, 0, 1)) {
                        audio->setDoppler(d);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Channel Doppler strength.");
                    }

                    if (ImGui::SliderFloat2("Falloff", f, 0, AUDIO_MAX_FALLOFF_MAX)) {
                        f[0] = std::min(f[0], f[1]);
                        audio->setFalloff(f[0], f[1]);
                    }
                    if (ImGui::IsItemHovered()) {
                        std::string tag("Channel Min and max 3D falloff. Default: Min " + std::to_string(AUDIO_MIN_FALLOFF_DEFAULT) + " / Max " + std::to_string(AUDIO_MAX_FALLOFF_MAX) + ".");
                        ImGui::SetTooltip(tag.c_str());
                    }

                    if (ImGui::SliderFloat("3D Level", &l, 0, 1)) {
                        audio->set3DLevel(l);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Channel 3D strength.");
                    }
                    int index = 0;

                    ImGui::NewLine();

                    std::string tag = std::string("Is playing? : ") + ((audio->isPlaying()) ? "true" : "false");
                    ImGui::Text(tag.c_str());

                    ImGui::NewLine();

                    ImGui::BeginListBox("##AudioClips");
                    ImGui::Text("Audio Clips");
                    int selectedClipIndex = -1;
                    bool isSelectedClipSet = false;
                    for (int i = 0; i < MAX_CLIPS; i++) {
                        if (audio->m_clips[i] != nullptr) {
                            index++;
                            if (m_selectedAudioClip == audio->m_clips[i]) {
                                selectedClipIndex = i;
                                isSelectedClipSet = true;
                                ImGui::Text(">");
                                ImGui::SameLine();
                            }

                            if (ImGui::Button(audio->m_clips[i]->getName().c_str())) {
                                //audio->playSound(i);
                                m_selectedAudioClip = audio->m_clips[i];
                                isSelectedClipSet = true;
                                selectedClipIndex = i;
                            }
                        }
                    }
                    if (!isSelectedClipSet)
                        m_selectedAudioClip = nullptr;
                    ImGui::EndListBox();
                    if (index != MAX_CLIPS) {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        if (ImGui::Button("+##CreateAudioClip")) {
                            m_fileDiag.Open();
                            m_fileDiag.ClearSelected();
                        }
                        if (m_fileDiag.HasSelected()) {
                            std::filesystem::path file = m_fileDiag.GetSelected();
                            std::string ext = File::getExtension(file.string());
                            if (ext == "wav" || ext == "ogg" || ext == "mp3") {
                                AudioClip * ac = new AudioClip();
                                if (ac->createSound(file.string().c_str()))
                                    audio->m_clips[index] = ac;
                                else{
                                    ac = AudioClip::getClipViaName(file.filename().string());
                                    if (ac) {
                                        std::cout << "AudioClip already exists - getting existing clip by that name instead." << std::endl;
                                        audio->m_clips[index] = ac;
                                    }
                                    else {
                                        delete ac;
                                    }
                                }
                            }
                            m_fileDiag.ClearSelected();
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Create a new AudioClip from a file");
                        }
                        ImGui::SameLine();
                        if (selectedClipIndex != -1) {
                            if (ImGui::Button("-##DestroyAudioClip")) {
                                m_selectedAudioClip->destroy();
                                audio->m_clips[selectedClipIndex] = nullptr;
                                m_selectedAudioClip = nullptr;
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Destroy the audio clip");
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Play##PlayAudioClip")) {
                                audio->playSound(m_selectedAudioClip);
                            }
                            ImGui::SameLine();
                            if (audio->isPlaying()) {
                                if (ImGui::Button("Stop##StopAudioClip")) {
                                    audio->stopAudio();
                                }
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Play the selected audio clip");
                            }
                        }


                    }
                }
            }
            #pragma endregion

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
        //
        ImGui::Text(" ");
        ImGui::Text(" ");
    }
    ImGui::EndChild();
    ImGui::End();

    if (m_createPopup) {

        ImGui::Begin(m_popupTitle.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        if (m_createPopupFF) {
            ImGui::SetWindowFocus();
            ImGui::SetWindowPos({Mouse::get().rawPos().x, Mouse::get().rawPos().y});
            ImGui::SetWindowPos({ImGui::GetWindowPos().x - (ImGui::GetWindowWidth() / 2), ImGui::GetWindowPos().y - (ImGui::GetWindowHeight() / 2)});
        }

        ImGui::Text(m_popupText.c_str());
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - 15);
        if (ImGui::Button("OK", {30, 30})) {
            m_createPopup = false;
        }
        m_createPopupFF = false;
        ImGui::End();
    }
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
            for (int i = 0; i < RenderEntity::s_allRenderEntities.size(); i++) {
                Sprite* spr = dynamic_cast<Sprite*>(RenderEntity::s_allRenderEntities[i]);
                if (spr) {
                    if (spr->checkCollisionAgainstPoint(Mouse::get().screenPos()) && spr->m_visible && !spr->m_selected) {
                        m_rightClickedObject = spr;
                        break;
                    }
                }
                else {
                    ScriptableEntity* ent = dynamic_cast<ScriptableEntity*>(RenderEntity::s_allRenderEntities[i]);
                    if (ent) {
                        if (ent->transform.checkCollisionAgainstPoint(Mouse::get().screenPos()) && ent->m_visible && !ent->m_selected) {
                            m_rightClickedObject = ent;
                            break;
                        }
                    }
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

                    if (ImGui::Button("AudioSource Entity")) {
                        AudioSource* n = new AudioSource;
                        n->create("AudioSource Parent ");
                        ScriptableEntity* p = ent->getParent();
                        if (p != nullptr) n->setParent(p);
                        ent->setParent(n);
                        closeFlag = true;
                    };

                    if (ImGui::Button("TextRenderer Entity")) {
                        TextRenderer* t = new TextRenderer;
                        t->create("TextRenderer Parent ");
                        ScriptableEntity* p = ent->getParent();
                        if (p != nullptr) t->setParent(p);
                        ent->setParent(t);
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

                    if (ImGui::Button("AudioSource Entity")) {
                        AudioSource* n = new AudioSource;
                        n->create("AudioSource Child ");
                        ent->addChild(n);
                        closeFlag = true;
                    }

                    if (ImGui::Button("TextRenderer Entity")) {
                        TextRenderer* t = new TextRenderer;
                        t->create("TextRenderer Child ");
                        ent->addChild(t);
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

                if (ImGui::Button("AudioSource Entity")) {
                    AudioSource* n = new AudioSource;
                    n->create("AudioSource Entity ");
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        m_selectedSprites[i]->m_selected = false;
                    }
                    m_selectedSprites.clear();
                    n->m_selected = true;
                    m_selectedSprites.push_back(n);
                    closeFlag = true;
                }

                if (ImGui::Button("TextRenderer Entity")) {
                    TextRenderer* t = new TextRenderer;
                    t->create("TextRenderer Entity ");
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        m_selectedSprites[i]->m_selected = false;
                    }
                    m_selectedSprites.clear();
                    t->m_selected = true;
                    m_selectedSprites.push_back(t);
                    closeFlag = true;
                }

                ImGui::EndMenu();

            }

            if (m_selectedSprites.size() > 0) {
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y + 5});
                if (ImGui::Button((m_selectedSprites.size() > 1) ? "Delete Entities" : "Delete Entity")) {
                    for (int i = 0; i < m_selectedSprites.size(); i++) {
                        GameLevel::deleteEntity(m_selectedSprites[i]);
                        m_selectedSprites.erase(m_selectedSprites.begin() + i);
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
                ImGui::SetCursorPos({ImGui::GetCursorPos().x - 3, ImGui::GetCursorPos().y - 3});
                if (ImGui::Button("Focus")) {
                    Camera::mainCamera->transform.setGlobalPositionX(m_selectedSprites[0]->transform.getGlobalPosition().x);
                    Camera::mainCamera->transform.setGlobalPositionY(m_selectedSprites[0]->transform.getGlobalPosition().y);
                    Camera::mainCamera->m_zoomFactor = Camera::mainCamera->m_zoomFactorDefault;
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

        if (IsItemDroppedHere()) {
            std::string ext = File::getExtension(m_draggedItemLF.string());
            if (ext == "ttf" || ext == "otf") {
                TextFont* font = TextFont::loadFont(m_draggedItemLF.string(), -1);
                if (font) {
                    TextRenderer* text = new TextRenderer;
                    text->create(m_draggedItemLF.filename().string());
                    text->setFont(font);
                    text->setText(m_draggedItemLF.filename().string());
                }
            }
            for (int i = 0; i < m_recognisedImageFormatExts.size(); i++) {
                if (m_recognisedImageFormatExts[i] == File::getExtension(m_draggedItemLF.string())) {
                    const char* path[1];
                    std::string file = m_draggedItemLF.string();
                    path[0] = file.c_str();
                    drop_callback(WindowManager::get()->m_wnd, 1, path);
                    float xpos = (((Mouse::get().screenPos().x / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition().x));
                    float ypos = (((Mouse::get().screenPos().y / Camera::mainCamera->m_zoomFactor) + Camera::mainCamera->transform.getGlobalPosition().y));
                    m_entityJustDropped->transform.setGlobalPosition(glm::vec3(xpos, ypos, m_entityJustDropped->transform.getGlobalPosition().z));
                    break;
                }
            }
        }

        if (ImGui::ImageButton("##materialEditor", m_materialIcon.getID(), {64, 64}, {0, 1}, {1, 0})) {
            m_openMaterialEditor = !m_openMaterialEditor;
            m_materialEditorForSwapping = false;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Material Editor");

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);
        if (ImGui::ImageButton("##channelMixer", m_channelMixerIcon.getID(), {64, 64}, {0, 1}, {1, 0})) {
            m_openChannelMixer = !m_openChannelMixer;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Channel Mixer");


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

    if (m_openChannelMixer) {
        if(ImGui::Begin("Channel Mixers")) {
            if (ImGui::Button("Create New")) {
                ChannelGroup* newGroup = new ChannelGroup("New Group");
            }
            for (int i = 0; i < ChannelGroup::s_channelGroups.size(); i++) {
                ChannelGroup* cg = ChannelGroup::s_channelGroups[i];
                float winSize = ImGui::GetWindowSize().x - 50;
                ImGui::SetNextWindowSize({(winSize) / 2, ImGui::GetWindowSize().y});
                ImGui::BeginChild((std::string(cg->getName().c_str()) + std::string("##_")).c_str());
                if (ImGui::CollapsingHeader(("##" + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf)) {
                    char nameBuf[64]{0};
                    for (int j = 0; j < std::min(static_cast<int>(cg->getName().size()), 64); j++) {
                        nameBuf[j] = cg->getName()[j];
                    }
                    std::string textTag = "##name" + std::to_string(i);
                    ImGui::InputText(textTag.c_str(), nameBuf, 64);
                    if (ImGui::IsItemFocused() && (glfwGetKey(WindowManager::get()->m_wnd, GLFW_KEY_ENTER) == GLFW_TRUE || ImGui::IsMouseClicked(ImGuiMouseButton_Left))) {
                        cg->setName(nameBuf);
                    }
                    if (ImGui::Button("Delete")) {
                        delete cg;
                    }
                    else {
                        float v = cg->getVolume() * 100;
                        float p = cg->getPitch() - 1;
                        float d = cg->getDoppler();
                        float l = cg->get3DLevel();
                        float f[2] = {cg->getMinFalloff(), cg->getMaxFalloff()};

                        float sliderSize = (ImGui::GetWindowSize().x - 50) / 5;

                        // TODO : Way to create and destroy (and maybe prompt user on destruction of) channel groups

                        if (ImGui::VSliderFloat((std::string("##ChannelGroupVolume") + cg->getName()).c_str(), {sliderSize, ImGui::GetWindowSize().y - 70}, &v, 0, 300, "%.1f")) {
                            cg->setVolume(v / 100);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Volume (0, 300)");
                        }
                        ImGui::SameLine();

                        if (ImGui::VSliderFloat((std::string("##ChannelGroupPitch") + cg->getName()).c_str(), {sliderSize, ImGui::GetWindowSize().y - 70}, &p, -1, 1)) {
                            cg->setPitch(p + 1);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Pitch (-1, 1)");
                        }
                        ImGui::SameLine();

                        if (ImGui::VSliderFloat((std::string("##ChannelGroupDoppler") + cg->getName()).c_str(), {sliderSize, ImGui::GetWindowSize().y - 70}, &d, 0, 1)) {
                            cg->setDoppler(d);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Doppler (0, 1)");
                        }
                        ImGui::SameLine();

                        ImVec2 pos = ImGui::GetCursorPos();
                        if (ImGui::VSliderFloat((std::string("##ChannelGroupMaxFalloff") + cg->getName()).c_str(), {sliderSize, ((ImGui::GetWindowSize().y - 70) / 2) - 5}, &f[1], 100, AUDIO_MAX_FALLOFF_MAX)) {
                            if (f[0] > f[1]) f[0] = f[1];
                            cg->setFalloff(f[0], f[1]);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip((std::string("Minimum Max Falloff (0, ") + std::to_string(AUDIO_MAX_FALLOFF_MAX) + std::string(")")).c_str());
                        }
                        ImGui::SetCursorPos({pos.x, pos.y + ((ImGui::GetWindowSize().y - 70) / 2) + 5});
                        if (ImGui::VSliderFloat((std::string("##ChannelGroupMinFalloff") + cg->getName()).c_str(), {sliderSize, ((ImGui::GetWindowSize().y - 70) / 2) - 5}, &f[0], 0, f[1])) {
                            if (f[0] > f[1]) f[0] = f[1];
                            cg->setFalloff(f[0], f[1]);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip((std::string("Maximum Min Falloff (0, ") + std::to_string(static_cast<int>(f[1])) + std::string(")")).c_str());
                        }
                        ImGui::SameLine();
                        ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - ((ImGui::GetWindowSize().y - 70) / 2) - 5});
                        if (ImGui::VSliderFloat((std::string("##ChannelGroup3DLevel") + cg->getName()).c_str(), {sliderSize, ImGui::GetWindowSize().y - 70}, &l, 0, 1)) {
                            cg->set3DLevel(l);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("3D Level (0, 1)");
                        }
                    }
                }

                ImGui::EndChild();
                if (i % 3 == 0)
                    ImGui::SameLine();

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

void EditorUI::scaleAlongN() {
    for (int i = 0; i < m_selectedSprites.size(); i++) {
        Sprite* spr = dynamic_cast<Sprite*>(m_selectedSprites[i]);
        if (!spr) return;
        float delta = (Mouse::get().screenPos().y - Mouse::get().screenPosLF().y) / Camera::mainCamera->m_zoomFactor;
        float _ = spr->trueScaleGlobal().y;
        spr->transform.setGlobalScaleY(spr->transform.getGlobalScale().y + (delta / ((spr->m_material->m_diffuseTexture) ? spr->m_material->m_diffuseTexture->dimensions().y: 1)));
        spr->transform.translateGlobalPositionY((spr->trueScaleGlobal().y - _) / 2);
    }
}

void EditorUI::scaleAlongE() {
    for (int i = 0; i < m_selectedSprites.size(); i++) {
        Sprite* spr = dynamic_cast<Sprite*>(m_selectedSprites[i]);
        if (!spr) return;
        float delta = (Mouse::get().screenPos().x - Mouse::get().screenPosLF().x) / Camera::mainCamera->m_zoomFactor;
        float _ = spr->trueScaleGlobal().x;
        spr->transform.setGlobalScaleX(spr->transform.getGlobalScale().x + (delta / ((spr->m_material->m_diffuseTexture) ? spr->m_material->m_diffuseTexture->dimensions().x: 1)));
        spr->transform.translateGlobalPositionX((spr->trueScaleGlobal().x - _) / 2);
    }
}

void EditorUI::scaleAlongS() {
    for (int i = 0; i < m_selectedSprites.size(); i++) {
        Sprite* spr = dynamic_cast<Sprite*>(m_selectedSprites[i]);
        if (!spr) return;
        float delta = -(Mouse::get().screenPos().y - Mouse::get().screenPosLF().y) / Camera::mainCamera->m_zoomFactor;
        float _ = spr->trueScaleGlobal().y;
        spr->transform.setGlobalScaleY(spr->transform.getGlobalScale().y + (delta / ((spr->m_material->m_diffuseTexture) ? spr->m_material->m_diffuseTexture->dimensions().y: 1)));
        spr->transform.translateGlobalPositionY(-(spr->trueScaleGlobal().y - _) / 2);
    }
}

void EditorUI::scaleAlongW() {
    for (int i = 0; i < m_selectedSprites.size(); i++) {
        Sprite* spr = dynamic_cast<Sprite*>(m_selectedSprites[i]);
        if (!spr) return;
        float delta = -(Mouse::get().screenPos().x - Mouse::get().screenPosLF().x) / Camera::mainCamera->m_zoomFactor;
        float _ = spr->trueScaleGlobal().x;
        spr->transform.setGlobalScaleX(spr->transform.getGlobalScale().x + (delta / ((spr->m_material->m_diffuseTexture) ? spr->m_material->m_diffuseTexture->dimensions().x: 1)));
        spr->transform.translateGlobalPositionX(-(spr->trueScaleGlobal().x - _) / 2);
    }
}

void EditorUI::update() {

    GLFWwindow* m_wnd = WindowManager::get()->m_wnd;

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
            GameLevel::deleteEntity(m_selectedSprites[i]);
            m_selectedSprites.erase(m_selectedSprites.begin() + i);
        }
        m_selectedSprites.clear();
    }

    m_mousePressed = false;
    static bool hoverFlag = false;

    if (m_viewportHasMouse && m_draggedItem == "") {
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

            else if ((m_axisHandle.m_nHandle) && !m_multiSelect) {
                m_axisHandle.m_nHandleSelected = true;
                scaleAlongN();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNS);
            }
            else if ((m_axisHandle.m_eHandle) && !m_multiSelect) {
                m_axisHandle.m_eHandleSelected = true;
                scaleAlongE();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorWE);
            }
            else if ((m_axisHandle.m_sHandle) && !m_multiSelect) {
                m_axisHandle.m_sHandleSelected = true;
                scaleAlongS();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNS);
            }
            else if ((m_axisHandle.m_wHandle) && !m_multiSelect) {
                m_axisHandle.m_wHandleSelected = true;
                scaleAlongW();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorWE);
            }
            else if ((m_axisHandle.m_neHandle) && !m_multiSelect) {
                m_axisHandle.m_neHandleSelected = true;
                scaleAlongN();
                scaleAlongE();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNESW);
            }
            else if ((m_axisHandle.m_seHandle) && !m_multiSelect) {
                m_axisHandle.m_seHandleSelected = true;
                scaleAlongE();
                scaleAlongS();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNESW);
            }
            else if ((m_axisHandle.m_swHandle) && !m_multiSelect) {
                m_axisHandle.m_swHandleSelected = true;
                scaleAlongS();
                scaleAlongW();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNESW);
            }
            else if ((m_axisHandle.m_nwHandle) && !m_multiSelect) {
                m_axisHandle.m_nwHandleSelected = true;
                scaleAlongW();
                scaleAlongN();
                Mouse::get().setMouseCursor(Mouse::get().s_moveCursorNESW);
            }

            if (Mouse::get().screenPos() != m_mouseScreenPosWhenPressed && !(
            m_axisHandle.m_xHandleSelected  ||
            m_axisHandle.m_yHandleSelected  ||
            m_axisHandle.m_xyHandleSelected ||
            m_axisHandle.m_nHandleSelected  ||
            m_axisHandle.m_neHandleSelected ||
            m_axisHandle.m_eHandleSelected  ||
            m_axisHandle.m_seHandleSelected ||
            m_axisHandle.m_sHandleSelected  ||
            m_axisHandle.m_swHandleSelected ||
            m_axisHandle.m_wHandleSelected  ||
            m_axisHandle.m_nwHandleSelected
        )) {
                m_multiSelect = true;
            }
        }


        if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && m_mousePressedLF) {

            if ((m_axisHandle.m_xHandleSelected ||
            m_axisHandle.m_yHandleSelected ||
            m_axisHandle.m_xyHandleSelected ||
            m_axisHandle.m_nHandleSelected ||
            m_axisHandle.m_neHandleSelected ||
            m_axisHandle.m_eHandleSelected ||
            m_axisHandle.m_seHandleSelected ||
            m_axisHandle.m_sHandleSelected ||
            m_axisHandle.m_swHandleSelected ||
            m_axisHandle.m_wHandleSelected ||
            m_axisHandle.m_nwHandleSelected
        )) {
                m_axisHandle.m_xHandleSelected = false;
                m_axisHandle.m_yHandleSelected = false;
                m_axisHandle.m_xyHandleSelected = false;
                m_axisHandle.m_nHandleSelected = false;
                m_axisHandle.m_neHandleSelected = false;
                m_axisHandle.m_eHandleSelected = false;
                m_axisHandle.m_seHandleSelected = false;
                m_axisHandle.m_sHandleSelected = false;
                m_axisHandle.m_swHandleSelected = false;
                m_axisHandle.m_wHandleSelected = false;
                m_axisHandle.m_nwHandleSelected = false;
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
                    for (int i = 0; i < RenderEntity::s_allRenderEntities.size(); i++) {
                        if (!earlyBreak) {
                            bool condition = false;

                            ScriptableEntity* ent = dynamic_cast<ScriptableEntity*>(RenderEntity::s_allRenderEntities[i]);
                            Sprite* spr = dynamic_cast<Sprite*>(ent);
                            if (spr) {
                                if (!m_multiSelect) condition = spr->checkCollisionAgainstPoint(Mouse::get().screenPos());
                                else                condition = spr->checkCollisionAgainstTransform(&m_selectionBox.transform);
                            }
                            else{
                                if (!m_multiSelect) condition = ent->transform.checkCollisionAgainstPoint(Mouse::get().screenPos());
                                else                condition = ent->transform.checkCollisionAgainstTransform(&m_selectionBox.transform);
                            }
                            if (condition && ent->m_visible && !ent->m_selected) {
                                ent->m_selected = true;
                                m_selectedSprites.push_back(ent);
                                if (!m_multiSelect) earlyBreak = true;
                            }
                        }
                    }
                }
                m_multiSelect = false;
            }
        }

    }

    if (m_selectedSprites.size() != 0) {
        glm::vec3 pos{};

        glm::vec2 N, E, S, W, newN, newE, newS, newW;
        int i;
        for (i = 0; i < m_selectedSprites.size(); i++) {
            pos += m_selectedSprites[i]->transform.getGlobalPosition();

            Sprite* spr = dynamic_cast<Sprite*>(m_selectedSprites[i]);
            if (spr) {
                bool flag = false;
                newN = spr->getN();
                newE = spr->getE();
                newS = spr->getS();
                newW = spr->getW();
                if (i == 0) flag = true;
                if (newN.y < spr->transform.getGlobalPosition().y) flag = true;
                if (newE.x < spr->transform.getGlobalPosition().x) flag = true;

                if (newN.y > N.y || flag) N.y = newN.y;
                if (newE.x > E.x || flag) E.x = newE.x;
                if (newS.y < S.y || flag) S.y = newS.y;
                if (newW.x < W.x || flag) W.x = newW.x;
            }
            else {
                N = glm::vec2(m_selectedSprites[i]->transform.getGlobalPosition().x, m_selectedSprites[i]->transform.getGlobalPosition().y);
                E = glm::vec2(m_selectedSprites[i]->transform.getGlobalPosition().x, m_selectedSprites[i]->transform.getGlobalPosition().y);
                S = glm::vec2(m_selectedSprites[i]->transform.getGlobalPosition().x, m_selectedSprites[i]->transform.getGlobalPosition().y);
                W = glm::vec2(m_selectedSprites[i]->transform.getGlobalPosition().x, m_selectedSprites[i]->transform.getGlobalPosition().y);
            }
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

        m_axisHandle.n.setLocalPosition(glm::vec3(pos.x, N.y, 0));
        m_axisHandle.ne.setLocalPosition(glm::vec3(E.x, N.y, 0));
        m_axisHandle.e.setLocalPosition(glm::vec3(E.x, pos.y, 0));
        m_axisHandle.se.setLocalPosition(glm::vec3(E.x, S.y, 0));
        m_axisHandle.s.setLocalPosition(glm::vec3(pos.x, S.y, 0));
        m_axisHandle.sw.setLocalPosition(glm::vec3(W.x, S.y, 0));
        m_axisHandle.w.setLocalPosition(glm::vec3(W.x, pos.y, 0));
        m_axisHandle.nw.setLocalPosition(glm::vec3(W.x, N.y, 0));


        if (!(m_axisHandle.m_xHandleSelected ||
            m_axisHandle.m_yHandleSelected ||
            m_axisHandle.m_xyHandleSelected ||
            m_axisHandle.m_nHandleSelected ||
            m_axisHandle.m_neHandleSelected ||
            m_axisHandle.m_eHandleSelected ||
            m_axisHandle.m_seHandleSelected ||
            m_axisHandle.m_sHandleSelected ||
            m_axisHandle.m_swHandleSelected ||
            m_axisHandle.m_wHandleSelected ||
            m_axisHandle.m_nwHandleSelected
        )) {
            m_axisHandle.m_xHandle = m_axisHandle.tx.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
            m_axisHandle.m_yHandle = m_axisHandle.ty.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
            m_axisHandle.m_xyHandle = m_axisHandle.transformxy.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);

            AudioSource * test = dynamic_cast<AudioSource*>(m_selectedSprites[0]);
            if (!test) {
                m_axisHandle.m_nHandle = m_axisHandle.n.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_neHandle = m_axisHandle.ne.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_eHandle = m_axisHandle.e.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_seHandle = m_axisHandle.se.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_sHandle = m_axisHandle.s.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_swHandle = m_axisHandle.sw.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_wHandle = m_axisHandle.w.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                m_axisHandle.m_nwHandle = m_axisHandle.nw.checkCollisionAgainstPoint(Mouse::get().screenPos(), true);
                }
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

            glm::vec3 scale = ((glm::vec3(20, 20, 1) / Camera::mainCamera->m_zoomFactor)) / 1.5f;
            if (m_axisHandle.m_nHandle && !m_multiSelect) {
                m_axisHandle.n.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.n.setLocalScale(scale);
            }

            if (m_axisHandle.m_neHandle && !m_multiSelect) {
                m_axisHandle.ne.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.ne.setLocalScale(scale);
            }

            if (m_axisHandle.m_eHandle && !m_multiSelect) {
                m_axisHandle.e.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.e.setLocalScale(scale);
            }

            if (m_axisHandle.m_seHandle && !m_multiSelect) {
                m_axisHandle.se.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.se.setLocalScale(scale);
            }

            if (m_axisHandle.m_sHandle && !m_multiSelect) {
                m_axisHandle.s.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.s.setLocalScale(scale);
            }

            if (m_axisHandle.m_swHandle && !m_multiSelect) {
                m_axisHandle.sw.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.sw.setLocalScale(scale);
            }

            if (m_axisHandle.m_wHandle && !m_multiSelect) {
                m_axisHandle.w.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.w.setLocalScale(scale);
            }

            if (m_axisHandle.m_nwHandle && !m_multiSelect) {
                m_axisHandle.nw.setLocalScale(scale * 1.2f);
                hoverFlag = true;
            }
            else {
                m_axisHandle.nw.setLocalScale(scale);
            }

            if (!(
                m_axisHandle.m_xHandle||
                m_axisHandle.m_yHandle  ||
                m_axisHandle.m_xyHandle ||
                m_axisHandle.m_nHandle  ||
                m_axisHandle.m_neHandle ||
                m_axisHandle.m_eHandle  ||
                m_axisHandle.m_seHandle ||
                m_axisHandle.m_sHandle  ||
                m_axisHandle.m_swHandle ||
                m_axisHandle.m_wHandle  ||
                m_axisHandle.m_nwHandle))
                hoverFlag = false;
        }

    }

    if (hoverFlag) {
        Mouse::get().setMouseCursor(Mouse::get().s_handCursor);
    }
    else
        Mouse::get().setMouseCursor(Mouse::get().s_defaultCursor);


    scroll_lf = Camera::mainCamera->m_zoomFactor;
    m_mousePressedLF = m_mousePressed;
    imgui();
    m_draggedItemLF = m_draggedItem;
}

bool EditorUI::drawUI() {
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

        m_axisHandle.n.calculateWorldMatrix();
        m_axisHandle.ne.calculateWorldMatrix();
        m_axisHandle.e.calculateWorldMatrix();
        m_axisHandle.se.calculateWorldMatrix();
        m_axisHandle.s.calculateWorldMatrix();
        m_axisHandle.sw.calculateWorldMatrix();
        m_axisHandle.w.calculateWorldMatrix();
        m_axisHandle.nw.calculateWorldMatrix();

        m_axisHandle.m_mesh.bind();

        //glDisable(GL_DEPTH_TEST);
        m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.transform.localMatrix() * m_axisHandle.transformx.localMatrix(), modelUniform);
        m_axisHandle.m_material.m_shader->setVec4({1, 0, 0, 1}, "handleCol");
        glDrawElements(m_axisHandle.m_mesh.m_topology, m_axisHandle.m_mesh.m_indexData.size(), GL_UNSIGNED_INT, 0);

        m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.transform.localMatrix() * m_axisHandle.transformy.localMatrix(), modelUniform);
        m_axisHandle.m_material.m_shader->setVec4({0, 0, 1, 1}, "handleCol");
        glDrawElements(m_axisHandle.m_mesh.m_topology, m_axisHandle.m_mesh.m_indexData.size(), GL_UNSIGNED_INT, 0);

        // always bind the right mesh before drawing lol. lol. 3am btw.
        Mesh::Quad.bind();
        Sprite* test = dynamic_cast<Sprite*>(m_selectedSprites[0]);
        if (test) {

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.n.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.ne.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.e.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.se.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.s.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.sw.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.w.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

            m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.nw.localMatrix(), modelUniform);
            m_axisHandle.m_material.m_shader->setVec4({1, 0.5f, 0, 1}, "handleCol");
            glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);
        }

        m_axisHandle.m_material.m_shader->setMat4(m_axisHandle.transformxy.localMatrix(), modelUniform);
        m_axisHandle.m_material.m_shader->setVec4({0, 1, 0, 1}, "handleCol");
        glDrawElements(Mesh::Quad.m_topology, Mesh::Quad.m_indexData.size(), GL_UNSIGNED_INT, 0);

        // dont look

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

    m_axisHandle.m_material.m_serialize = false;
    m_axisHandle.m_mesh.setupMesh(v, ind);
    m_axisHandle.m_material.m_shader = ShaderManager::get().s_shaderList[0];
    m_axisHandle.transformx.setLocalRotationZ(90);
}
