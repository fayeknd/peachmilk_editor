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
    /*FLAG;*/\
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

    // BIG WALL OF TEXT !! INTERESTING! READ!

    // okay, update on this
    // I feel the need to keep this here so anyone who
    // sees this can know how much of an idiot i am.
    //
    // The reason this was happening (which is so fucking
    // obvious in hindsight) is because deleting an entity
    // also deletes it's children, and then erases the child
    // fropm the s_loadedEntities vector.
    // This creates a problem in SPECIFICALLY this loop,
    // because after erasing the item, iterator i skips
    // to the entity immediately after the one it was meant to.
    // Creating a separate vector called ent did kind of fix this
    // until it tried deleting pointers that had already been
    // deleted, where it caused issues. The simple simple simple
    // fix was to just reverse the order of the for loop. omg.

    // begin old rant

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

    // end old rant

    std::vector<ScriptableEntity*> ent = s_loadedEntities;
    for (int i = s_loadedEntities.size() - 1; i >= 0; --i) {

        if (s_loadedEntities[i]->m_destroyOnLoad) {
            deleteEntity(s_loadedEntities[i]);
        }

    }
    //Material::serializeMaterials();
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
        Shader::m_clearCol[0] = gl->m_clearCol.x;
        Shader::m_clearCol[1] = gl->m_clearCol.y;
        Shader::m_clearCol[2] = gl->m_clearCol.z;
        Shader::m_clearCol[3] = gl->m_clearCol.w;
        glClearColor(gl->m_clearCol.x, gl->m_clearCol.y, gl->m_clearCol.z, gl->m_clearCol.w);
        RenderEntity::orderEntitiesByZ(true);

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

    // ok full disclosure (later me) i fully could fix this but i cba

    std::string txt(std::string("{\n    ") + '"' + std::string("value0") + '"' + " : 0,\n    " + '"' + std::string("value1") + '"' + " : {\n        " + '"' + std::string("value0") + '"' + " : " + '"' + name + '"' + ",\n" + '"' + std::string("value1") + '"' + " : " + "0.172" + ",\n" + '"' + std::string("value2") + '"' + " : " + "0.172" + ",\n" + '"' + std::string("value3") + '"' + " : " + "0.172" + ",\n" + '"' + std::string("value4") + '"' + " : " + "1.0" + "\n }\n}");
    os << txt;
    os.close();

}
