#pragma once
#include "../headers.h"
#include "mesh.hpp"
#include "entity.hpp"
#include "../system/time.hpp"
#include "transform.hpp"
#include "material.hpp"

// TODO : Sprites do not have to contain one mesh each, it would be a lot more efficient
// if all sprites were batched together and drawn as instanced quads, with differing
// transforms and textures.

class Sprite : public ScriptableEntity, public RenderEntity {
private:

public:

    unsigned int m_matID = -1;

    glm::vec2 textureSize();
    glm::vec3 trueScaleGlobal();
    glm::vec3 trueScaleLocal();

    //Sprite();
    ~Sprite();
    Mesh* m_mesh = &Mesh::Quad;
    Material* m_material = Material::s_defaultMaterial;
    Mesh* getMesh() { return m_mesh; }
    void create(std::string name = "Sprite ") override;

    void update() override {
        ScriptableEntity::update();
        //danceTheSprite();
    }

    void setScaleToTexelSize();

    void danceTheSprite() {
        transform.setLocalPositionY(std::sin(Time::sinceStartup() * 15) / 8);
        transform.setLocalPositionX(-std::sin(Time::sinceStartup() * 7.5) / 15);
        transform.setLocalRotationZ(std::sin(Time::sinceStartup() * 7.5) * 10);
        //transform.setRotationX(std::sin(glfwGetTime() * 7.5) * 3);
        transform.setLocalRotationY(std::sin(Time::sinceStartup() * 7.5) * 10);
    }

    bool draw() override;

    bool checkCollisionAgainstPoint(glm::vec2 point, bool local = false);
    bool checkCollisionAgainstTransform(Transform* t);

    template<class Archive>
    void serialize(Archive & archive) {
        m_matID = m_material->getID();
        archive(cereal::base_class<ScriptableEntity>(this), m_matID);
    }

    void _deserializeFnc() override {
        ScriptableEntity::_deserializeFnc();
        if (m_matID != -1) m_material = Material::getMaterialViaID(m_matID);
        if (!m_material) m_matID = -1;
        RenderEntity::s_allRenderEntities.push_back(this);
    };

    glm::vec2 getTL();
    glm::vec2 getBR();
    glm::vec2 getBL();
    glm::vec2 getTR();

    glm::vec2 getN();
    glm::vec2 getS();
    glm::vec2 getE();
    glm::vec2 getW();

};
