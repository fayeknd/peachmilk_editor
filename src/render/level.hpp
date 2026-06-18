#pragma once
#include "entity.hpp"
#include "../headers.h"

class GameLevel{
private:
    std::string m_filePath = "";
public:
    static inline GameLevel* s_loadedLevel = nullptr;
    std::string m_levelName = "New Level";
    static inline std::vector<ScriptableEntity*> s_loadedEntities {};
    void saveLevelData();
    static void loadLevel(std::string path, bool additive = false);
    std::string getFilePath() { return m_filePath; }
    
    template <class Archive>
    void serialize(Archive & archive) {
        archive(m_levelName);
    }
    void unloadLevel(); 
    static void createNewLevel(std::string levelName = "New Level");
    ~GameLevel() {
        for (int i = 0; i < s_loadedEntities.size(); i++) {
            if (s_loadedEntities[i]->m_destroyOnLoad) {
                delete s_loadedEntities[i];
                s_loadedEntities.erase(s_loadedEntities.begin() + i);
            }
        }
    }
    static void deleteEntity(ScriptableEntity* entity);
};