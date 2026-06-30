#pragma once
#include "../headers.h"
#include "../render/entity.hpp"
#include "../render/sprite.hpp"
#include "../render/mesh.hpp"
#include "../../include/AirGuanZ/imfilebrowser.h"
#include "../render/camera.hpp"
#include "../render/blend.hpp"
#include "../audio/audio.hpp"
#include "../audio/audioclip.hpp"
#include "../audio/channelgroup.hpp"
#include "../render/text/text.hpp"

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

class EditorUI : public ScriptableEntity{
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
    Texture m_channelMixerIcon;
    Texture m_folderIcon;
    Texture m_unrecognisedFileIcon;

    std::string m_fbCurrentDir = "";
    std::string m_popupText = "";
    std::string m_popupTitle = "Hey!";
    std::filesystem::path m_draggedItem;
    std::filesystem::path m_draggedItemLF;

    // didnt used to be static cba to change the name everywhere
    static inline std::vector<std::string> m_recognisedImageFormatExts {
        "jpg", "jpeg", "gif", "png", "mgd"
    };

    ImGui::FileBrowser m_fileDiag;
    // this is no longer for just sprites, but whatevs
    std::vector<ScriptableEntity*> m_selectedSprites;
    bool m_settingsOpen = false;
    bool m_contextOpen = false;
    bool m_contextOpenTF = false;
    bool m_editingText = false;
    bool m_openMaterialEditor = false;
    bool m_openChannelMixer = false;
    bool m_materialEditorForSwapping = false;
    bool m_createPopup = false;
    bool m_createPopupFF = true;
    bool m_rcoSelectedInUI = false;
    bool m_viewportHasMouse = false;
    bool m_showHiddenFiles = false;
    bool m_showOnlyFolders = false;
    bool m_toolWndHovering = false;
    bool m_mouseDownFF = true;
    char m_replacementName[64] = "";

    int m_iconsPerRow = 18;
    int m_iconSize = 22;
    int m_fontSize = ENGINE_DEFTAULT_FONT_SIZE;

    ScriptableEntity* m_spriteNameEditing = nullptr;
    ScriptableEntity* m_rightClickedObject = nullptr;
    static inline ScriptableEntity* m_entityJustDropped = nullptr;

    Material* m_rightClickedMaterial = nullptr;
    Sprite* m_leftClickedSprite_mat = nullptr;
    AudioClip* m_selectedAudioClip = nullptr;
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
    void drawProbablyComplicatedFileBrowserTypeShi();
    void createPopup(std::string text, std::string title = "Hey!");
    bool IsItemDroppedHere();

    void setImGuiStyle(int hue07, int alt07, int nav07, int lit01 = 0, int compact01 = 0, int border01 = 1, int shape0123 = 1);

};
