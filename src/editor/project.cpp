#include "project.hpp"
#include "../render/material.hpp"
#include "../audio/audioclip.hpp"
#include "../audio/channelgroup.hpp"

#define _GET_FOLDER(root, folder) {                                             \
    std::string s(root + "\\" + folder);                                        \
    if (!std::filesystem::is_directory(s)) std::filesystem::create_directory(s);\
    return s;                                                                   \
}                                                                               \

#define GET_FOLDER(folder) _GET_FOLDER(getGameDataFolder(), folder)

std::string Project::getGameDataFolder() {_GET_FOLDER(m_projectPath, "game_data")}
std::string Project::getSoundFolder() {GET_FOLDER("sounds")}
std::string Project::getMaterialFolder() {GET_FOLDER("materials")}
std::string Project::getTextureFolder() {GET_FOLDER("textures")}
std::string Project::getLevelFolder() {GET_FOLDER("levels")}
std::string Project::getChannelGroupFolder() {_GET_FOLDER(getSoundFolder(), "channelgroups")}
void Project::setLoadedProject(Project* p) { 
    m_activeProject = p; 
}

void Project::createProject(std::string title) {
    m_projectTitle = title;
    serializeProjectInfo();
}
void Project::serializeProjectInfo() {
    std::string file = PROJECTS_RC;
    file += "/" + m_projectTitle;
    std::string _;
    for (auto c : file) {
        if (c == ' ' || c == '[' || c == ']')
            c = '_';
        _+=c;
    }
    m_projectPath = std::string(File::getWorkingDirectory()) + "\\" + _;
    std::filesystem::create_directory(_);
    std::ofstream os(_ + "/.pinfo", std::ios::binary);
    {
        cereal::JSONOutputArchive oarchive(os); // Create an output archive
        oarchive(*this);
    }
    os.close();
}
void Project::deserializeProjectInfo(std::string pInfoFile) {
    if (!File::fileExists(pInfoFile)) return;
    std::filesystem::path p(pInfoFile);
    std::ifstream is(pInfoFile, std::ios::binary);
    {
        cereal::JSONInputArchive iarchive(is);
        iarchive(*this);
        m_projectPath = File::getFullPath(p.parent_path().string().c_str());
        m_deserialized = true;
        Camera::mainCamera->transform.setGlobalPosition(m_campos);
        Camera::mainCamera->m_zoomFactor = m_zoom;
        
    }
    is.close();
}

bool Project::saveProject() {
    GameLevel::s_loadedLevel->saveLevelData();
    serializeProjectInfo();
    Material::serializeMaterials();
    AudioClip::serializeSounds();
    ChannelGroup::serializeChannelGroups();
    return true;
}

void Project::loadProject() {
    Material::deseralizeMaterials();
    AudioClip::deseralizeSounds();
    ChannelGroup::deseralizeChannelGroups();
    if (m_loadedLevelPath != "" && File::fileExists(m_loadedLevelPath)) {
        GameLevel::loadLevel(m_loadedLevelPath, false);
    }
    else {
        bool flag = false;
        for (auto entry : std::filesystem::directory_iterator(getLevelFolder())) {
            if (File::getExtension(entry.path().string()) == LEV_DEFAULT_EXT) {
                GameLevel::loadLevel(entry.path().string(), false);
                flag = true;
                break;
            }
        }
        if (!flag) {
            GameLevel::createNewLevel();
            GameLevel::loadLevel(getLevelFolder() + "\\New Level.lvd");
        }
    }
}