#include "project.hpp"
#include "../render/material.hpp"

std::string Project::getGameDataFolder() {
    std::string s(m_projectPath + "\\game_data");
    if (!std::filesystem::is_directory(s)) std::filesystem::create_directory(s);
    return s;
}
std::string Project::getMaterialFolder() {
    std::string s(getGameDataFolder() + "\\materials");
    if (!std::filesystem::is_directory(s)) std::filesystem::create_directory(s);
    return s;
}
std::string Project::getTextureFolder() {
    std::string s(getGameDataFolder() + "\\textures");
    if (!std::filesystem::is_directory(s)) std::filesystem::create_directory(s);
    return s;
}
std::string Project::getLevelFolder() {
    std::string s(getGameDataFolder() + "\\levels");
    if (!std::filesystem::is_directory(s)) std::filesystem::create_directory(s);
    return s;
}
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
        Camera::mainCamera->transform.setLocalPosition(m_campos);
        Camera::mainCamera->m_zoomFactor = m_zoom;
    }
    is.close();
}

bool Project::saveProject() {
    GameLevel::s_loadedLevel->saveLevelData();
    serializeProjectInfo();
    Material::serializeMaterials();
    return true;
}