#include "entity.hpp"
#include <sstream>
#include "sprite.hpp"

ScriptableEntity* ScriptableEntity::s_sceneEntity = new ScriptableEntity;

ScriptableEntity* ScriptableEntity::getEntityViaID(unsigned int id) {
    for (int i = 0; i < GameLevel::s_loadedEntities.size(); i++) {
        if (GameLevel::s_loadedEntities.at(i)->m_ID == id) return GameLevel::s_loadedEntities.at(i);
    }
    return nullptr;
}

void ScriptableEntity::create(std::string name) {
    if (m_initialised) return;
    GameLevel::s_loadedEntities.push_back(this);
    const void * address = static_cast<const void*>(this);
    std::stringstream ss;
    ss << address;
    m_entityName = name + ss.str();
    if (s_availableID <= s_largestIDused) s_availableID = s_largestIDused + 1;
    m_ID = s_availableID;
    s_largestIDused = s_availableID;
    s_availableID++;
    for (int i = 0; i < GameLevel::s_loadedEntities.size(); i++) {
        if (GameLevel::s_loadedEntities[i]->m_ID == m_ID && GameLevel::s_loadedEntities[i] != this) {
            std::cout << "ERR : " << m_entityName << " has the same ID as another object! (" << GameLevel::s_loadedEntities[i]->m_entityName << ") this SHOULD NOT happen!" << std::endl;
            break;
        }
    }
    setParent(s_sceneEntity);
    m_initialised = true;
    start();
}

void ScriptableEntity::_deserializeFnc() {
    if (m_ID >= s_availableID) s_availableID = m_ID + 1;
}

ScriptableEntity::~ScriptableEntity() {
    setParent(nullptr);
    for (int i = m_children.size() - 1; i >= 0; --i) {
        Sprite* spr = dynamic_cast<Sprite*>(m_children[i]);
        if (spr) delete spr;
        else delete m_children[i];
    }
    for (int i = 0; i < GameLevel::s_loadedEntities.size(); i++) {
        if (GameLevel::s_loadedEntities[i] == this) {
            GameLevel::s_loadedEntities.erase(GameLevel::s_loadedEntities.begin() + i);
            return;
        }
    }

}

ScriptableEntity* ScriptableEntity::getChildAt(int index) {
    if (m_children.size() != 0) {
        if (m_children.size() - 1 < index) {
            std::cout << "ERROR : Children index " << index << " out of bounds! Returning top child" << std::endl;
            index = m_children.size() - 1;
        }
        return m_children[index];
    }
    std::cout << "ERROR : Children index is empty!" << std::endl;
    return nullptr;
}
void ScriptableEntity::addChild(ScriptableEntity* child) {
    child->setParent(this); // just a way to do it from the parent too.
}

bool ScriptableEntity::setParent(ScriptableEntity* parent) {
    //if (parent == nullptr) return false;
    if (m_parent != nullptr) {
        if (parent) {
            if (parent->m_parent == this) { // it can happen
                parent->setParent(m_parent);
            }
        }
        // if this entity has a parent, deregister this entity from parent's child index
        for (int i = 0; i < m_parent->m_children.size(); i++) {
            if (m_parent->m_children[i] == this) {
                m_parent->m_children.erase(m_parent->m_children.begin() + i);
                break;
            }
        }
    }
    if (parent) parent->m_children.push_back(this);
    m_parent = parent;
    transform.forceDirty();
    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->transform.forceDirty();
    }
    return true;
}

void ScriptableEntity::updateSelfAndChildTransforms(bool iter1, bool forceChildren) {
    // if this entity's transform is dirty, ensure all children's transforms also get updated.
    bool dirty = (transform.isDirty() || forceChildren);
    if (dirty)
        transform.calculateWorldMatrix((dynamic_cast<RenderEntity*>(this) != nullptr), (iter1) ? glm::mat4(1) : m_parent->transform.worldMatrix(), true);
    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->updateSelfAndChildTransforms(false, dirty);
    }
}

void ScriptableEntity::updateTransforms() {
    for (int i = 0; i < s_sceneEntity->m_children.size(); i++) {
        s_sceneEntity->m_children[i]->updateSelfAndChildTransforms(true);
    }
}

void ScriptableEntity::start() {};
void ScriptableEntity::update() {};
bool ScriptableEntity::preDraw() {

    ScriptableEntity* parent = getParent();
    if (parent != nullptr) {
        while (true) {
            if (!parent->m_visible) {
                return false;
            }
            ScriptableEntity* parent2 = parent->getParent();
            if (parent2 != nullptr) {
                parent = parent2;
            }
            else break;
        }
    }
    if (!m_visible) return false;
    return true;
}
