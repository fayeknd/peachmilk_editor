#pragma once
#include "../headers.h"
#include <fstream>
#include "../system/file.hpp"
#include <filesystem>
#include "../render/camera.hpp"
#include "../render/level.hpp"

class Project {
private:

    static inline Project* m_activeProject = nullptr;
    std::string m_projectPath = "";
    bool m_deserialized = false;
    glm::vec3 m_campos;
    float m_zoom;

public:

    static inline Project* get() { return m_activeProject; }

    std::string m_loadedLevelPath = "";
    std::string getGameDataFolder();
    std::string getSoundFolder();
    std::string getMaterialFolder();
    std::string getTextureFolder();
    std::string getLevelFolder();
    std::string getChannelGroupFolder();
    static void setLoadedProject(Project* p);
    bool isDeserialized() { return m_deserialized; }
    std::string m_projectTitle = "";

    template <class Archive> 
    void serialize(Archive & archive) {
        m_campos = Camera::mainCamera->transform.getLocalPosition();
        m_zoom = Camera::mainCamera->m_zoomFactor;
        archive(m_projectTitle, m_campos.x, m_campos.y, m_campos.z, m_zoom, m_loadedLevelPath);  
    }

    void createProject(std::string title = "Empty Project");
    void serializeProjectInfo();
    std::string getFilepath() { return m_projectPath; } 
    void deserializeProjectInfo(std::string pInfoFile);
    bool saveProject();
    void loadProject();

};