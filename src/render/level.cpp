#include "level.hpp"
#include "../editor/project.hpp"
#include <fstream>
#include "../system/serializable_types.h"

#define _SAVE_TYPE(Type) __SAVE_TYPE(Type, #Type)
#define __SAVE_TYPE(Type, typename){                            \
Type* t = dynamic_cast<Type*>(serializable[i]);                 \
if (t) {oarchive(std::string(#Type)); oarchive(*t); break;}}    \

#define _LOAD_TYPE(Type)                                        \
if (type == #Type) {                                            \
    Type* t = new Type;                                         \
    iarchive(*t);                                               \
    /*std::cout << t->transform.getLocalPosition().x << std::endl;*/\
    s_loadedEntities.push_back(t);                              \
    t->m_justDeserialized = true;                               \
    t->_deserializeFnc();                                       \
    t->setParent(ScriptableEntity::s_sceneEntity);              \
    break;                                                      \
} else                                                          \

#define _DELETE_TYPE(Type, ptr){                                \
Type* t = dynamic_cast<Type*>(ptr);                             \
if (t) {delete t;break;} }                                      \

void GameLevel::saveLevelData() {
    std::string path = Project::get()->getLevelFolder();
    path += "\\" + m_levelName + "." + LEV_DEFAULT_EXT;
    std::ofstream os(path, std::ios::binary);
    {
        std::vector<ScriptableEntity*> serializable {};
        if (s_loadedLevel == this) {
            for (int i = 0; i < s_loadedEntities.size(); i++) {
                if (s_loadedEntities[i]->m_serialize && s_loadedEntities[i]->m_destroyOnLoad) {
                    serializable.push_back(s_loadedEntities[i]);
                }
            }
            Project::get()->m_loadedLevelPath = path;
        }
        // this needs a precompile script to automatically generate potential casts
        cereal::JSONOutputArchive oarchive(os);
        oarchive(serializable.size());
        for (int i = 0; i < serializable.size(); i++) {
            SAVE_TYPES            
        }
        oarchive(*this);
    }   
}

void GameLevel::deleteEntity(ScriptableEntity *entity) {
    DELETE_ENTITY(entity);
}

void GameLevel::unloadLevel() {
    saveLevelData();
    
    // genuinely this system gives me the weirdest issues
    // i wrestled with this for an hour because if the 
    // "delete" keyword was present at all in this 
    // chain then the most recently created entity
    // would NOT be in the s_loadedEntities array
    // ******even if the delete was NEVER called******
    // this means the most recently created object
    // wouldn't be listed in this vector, wouldn't
    // get destroyed, but then would SUDDENLY exist in 
    // the SAME VECTOR once again when the next level 
    // saves its data, saving itself in that level's data.
    // i have NO. idea. why.
    
    // but if you just copy the pointers into this vector
    // it works fine 
    // i SWEAR im not missing something super obvious
    std::vector<ScriptableEntity*> ent = s_loadedEntities;
    for (int i = 0; i < ent.size(); i++) {

        if (ent[i]->m_destroyOnLoad) {
            deleteEntity(ent[i]);
        }
        
    }
    Material::serializeMaterials();
}

void GameLevel::loadLevel(std::string path, bool additive) {
    
    if (!File::fileExists(path)) return;
    if (s_loadedLevel && !additive) {
        s_loadedLevel->unloadLevel();
    }
    std::ifstream is(path, std::ios::binary);
    {
        cereal::JSONInputArchive iarchive(is);
        int size = 0;
        iarchive(size);
        std::string type;
        for (int i = 0; i < size; i++) {
            iarchive(type);
            LOAD_TYPES
        }
        
        for (int i = 0; i < s_loadedEntities.size(); i++) {
            if (s_loadedEntities[i]->m_justDeserialized) {
                ScriptableEntity* parent = ScriptableEntity::getEntityViaID(s_loadedEntities[i]->m_parentID);
                if (parent) {
                    s_loadedEntities[i]->setParent(parent);
                }
                s_loadedEntities[i]->m_justDeserialized = false;
            }
        }
        GameLevel* gl = new GameLevel;
        gl->m_filePath = path;
        iarchive(*gl);
        s_loadedLevel = gl;
        Project::get()->m_loadedLevelPath = gl->getFilePath();
        Sprite::orderSprites(true);
        
    }
}

void GameLevel::createNewLevel(std::string name) {
    std::string _ = Project::get()->getLevelFolder() + "\\" + name + "." + LEV_DEFAULT_EXT;
    std::ofstream os(_, std::ios::binary);
    // this is really fucking stupid but it was the only way
    // for some reason, if you created a level while a level 
    // with sprites was loaded, even though the (much better)
    // generation of a file had zero interaction with the
    // sprite vector, the sprites would still start refusing
    // to run their draw function even when the pointers were 
    // valid and they would do other functions just fine
    // this is genuinely the most puzzling thing ive ever
    // come across and clearly something is doing it
    // but i CANNOT figure out what it is so fuck it im 
    // just handwriting a new .lvd file cause fuck you
    std::string txt(std::string("{\n    ") + '"' + std::string("value0") + '"' + " : 0,\n    " + '"' + std::string("value1") + '"' + " : {\n        " + '"' + std::string("value0") + '"' + " : " + '"' + name + '"' + "\n  }\n}");
    os << txt;
    os.close();
}