#pragma once
#include <vector>
#include "transform.hpp"
#include "../system/generic.h"
#include "renderentity.hpp"

class ScriptableEntity : public Generic {
private:


protected:

    ScriptableEntity* m_parent = nullptr;
    std::vector<ScriptableEntity*> m_children;
    static inline unsigned int s_availableID = 0;
    static inline unsigned int s_largestIDused = 0;
    unsigned int m_ID = 0;
    bool m_initialised = false;

public:

    bool m_justDeserialized = false;
    unsigned int m_parentID;
    std::string m_entityName;
    static ScriptableEntity* s_sceneEntity;

    Transform transform;

    template <class Archive>
    void serialize(Archive & archive) {
        if (!m_serialize) return;
        m_parentID = (m_parent) ? m_parent->getID() : -1;
        archive(m_entityName, m_ID, m_parentID, transform, m_initialised, m_wireframe, m_visible);
    }

    virtual unsigned int getID() { return m_ID; }
    static ScriptableEntity* getEntityViaID(unsigned int id);
    virtual void create(std::string name = "Unnamed ");
    virtual void start();
    virtual void update();
    bool preDraw();
    static void updateTransforms();
    void updateSelfAndChildTransforms(bool iter1 = false, bool forceChildren = false);
    virtual void _deserializeFnc();
    virtual bool exists() { return m_initialised; }
    bool m_destroyOnLoad = true;

    virtual ScriptableEntity* getParent() { return m_parent; }
    virtual std::vector<ScriptableEntity*> getChildren() { return m_children; }
    virtual ScriptableEntity* getChildAt(int index);

    // parent->addChild() and child->setParent() are the exact same functions, but the choice to do it from either is fine enough
    // (this allows for stuff like parent->addChild(new Object))
    virtual bool setParent(ScriptableEntity* parent);
    virtual void addChild(ScriptableEntity* child);

    ~ScriptableEntity();

    bool m_hidden = false;
    bool m_wireframe = false;
    bool m_visible = true;
    bool m_selected = false;
    bool m_serialize = true;
};

CEREAL_REGISTER_TYPE(ScriptableEntity);
