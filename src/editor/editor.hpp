#pragma once
#include "../headers.h"
#include "../render/entity.hpp"
#include "../render/sprite.hpp"
#include "../render/mesh.hpp"
#include "../../include/AirGuanZ/imfilebrowser.h"
#include "../render/camera.hpp"
#include "../render/blend.hpp"
#include "../system/audio.hpp"

class AxisHandle {
private:
public:
    Mesh m_mesh; 
    Material m_material;

    Transform transformx;
    Transform transformy;
    Transform transformxy;
    Transform transform; 

    Transform tx, ty;

    Transform n, ne, e, se, s, sw, w, nw;

    int m_handleCount = 2;

    bool m_xHandle = false;
    bool m_yHandle = false;
    bool m_xyHandle = false;

    bool m_nHandle = false;
    bool m_neHandle = false;
    bool m_eHandle = false;
    bool m_seHandle = false;
    bool m_sHandle = false;
    bool m_swHandle = false; 
    bool m_wHandle = false;
    bool m_nwHandle = false;

    bool m_xHandleSelected = false;
    bool m_yHandleSelected = false;
    bool m_xyHandleSelected = false;

    bool m_nHandleSelected = false;
    bool m_neHandleSelected = false;
    bool m_eHandleSelected = false;
    bool m_seHandleSelected = false;
    bool m_sHandleSelected = false;
    bool m_swHandleSelected = false; 
    bool m_wHandleSelected = false;
    bool m_nwHandleSelected = false;


    glm::vec3 m_handlePosWhenPressed;
};
class SelectionBox : public ScriptableEntity {
private:
public:
    Mesh* m_mesh = &Mesh::Quad;
    Material m_material;
};

class EditorUI : public ScriptableEntity {
private:

    AxisHandle m_axisHandle;
    SelectionBox m_selectionBox;
    glm::vec3 m_mousePosWhenPressed;
    glm::vec3 m_mousePosWhenReleased;
    glm::vec3 m_mouseScreenPosWhenPressed;
    bool m_multiSelect = false;
    bool m_mousePressedLF = false;
    bool m_mousePressed = false;

public:

    EditorUI();
    ~EditorUI();

    ImVec2 m_viewportSize;

    Texture m_materialIcon;

    ImGui::FileBrowser m_fileDiag;
    std::vector<ScriptableEntity*> m_selectedSprites;
    bool m_settingsOpen = false;
    bool m_contextOpen = false;
    bool m_contextOpenTF = false;
    bool m_editingText = false;
    bool m_openMaterialEditor = false;
    bool m_materialEditorForSwapping = false;
    bool m_rcoSelectedInUI = false;
    bool m_viewportHasMouse = false;
    bool m_toolWndHovering = false;
    char m_replacementName[64] = "";
    ScriptableEntity* m_spriteNameEditing = nullptr;
    ScriptableEntity* m_rightClickedObject = nullptr;

    Material* m_rightClickedMaterial = nullptr;
    Sprite* m_leftClickedSprite_mat = nullptr;

    void update() override;
    bool drawUI();
    void _start();
    void imgui();
    void createAxisHandle();
    void create(std::string name = "Editor ") override;
    void deleteEntity(ScriptableEntity* entity);
    void moveAlongHandleX();
    void moveAlongHandleY();
    void scaleAlongN();
    void scaleAlongE();
    void scaleAlongS();
    void scaleAlongW();
    bool drawMaterialPreview(Material* mat, float size, bool inTextureMenu = true);
    bool drawEntityUI(ScriptableEntity* spr, float xOffset = 0, bool open = false);
    
};