#include "sprite.hpp"
#include "camera.hpp"
#include "../render/camera.hpp"
#include "entity.hpp"

Sprite::~Sprite() {
    for (int i = 0; i < RenderEntity::s_allRenderEntities.size(); i++) {
        if (RenderEntity::s_allRenderEntities[i] == this) {
            RenderEntity::s_allRenderEntities.erase(RenderEntity::s_allRenderEntities.begin() + i);
            break;
        }
    }
}

glm::vec2 Sprite::textureSize() {
    if (m_material->m_diffuseTexture) return m_material->m_diffuseTexture->dimensions();
    return {1,1};
}
glm::vec3 Sprite::trueScaleGlobal() {
    return glm::vec3(textureSize().x, textureSize().y, 1) * transform.getGlobalScale();
}
glm::vec3 Sprite::trueScaleLocal() {
    return glm::vec3(textureSize().x, textureSize().y, 1) * transform.getLocalScale();
}


bool Sprite::checkCollisionAgainstTransform(Transform* t) {
    glm::vec3 pos1 = transform.getGlobalPosition();
    glm::vec3 pos2 = t->getGlobalPosition();
    glm::vec3 size1 = trueScaleGlobal();
    glm::vec3 size2 = t->getGlobalScale();

    size1 = glm::vec3(std::abs(size1.x), std::abs(size1.y), 0);
    size2 = glm::vec3(std::abs(size2.x), std::abs(size2.y), 0);

    float w1 = pos1.x - size1.x / 2;
    float e1 = pos1.x + size1.x / 2;
    float n1 = pos1.y + size1.y / 2;
    float s1 = pos1.y - size1.y / 2;

    float w2 = pos2.x - size2.x / 2;
    float e2 = pos2.x + size2.x / 2;
    float n2 = pos2.y + size2.y / 2;
    float s2 = pos2.y - size2.y / 2;

    return (w1 < e2 && e1 > w2 && n1 > s2 && s1 < n2);

}

bool Sprite::checkCollisionAgainstPoint(glm::vec2 point, bool local) {
    glm::vec3 pos, scale;
    if (!local) {
        pos = transform.getGlobalPosition();
        scale = trueScaleGlobal();
    }
    else {
        pos = transform.getLocalPosition();
        scale = trueScaleLocal();
    }
    point = (point / Camera::mainCamera->m_zoomFactor);
    // point_worldPosition
    glm::vec3 point_wp = glm::vec3(point.x, point.y, 0) + Camera::mainCamera->transform.getGlobalPosition();
    // right, left, up, down
    float posX_r = pos.x + (scale.x / 2);
    float posX_l = posX_r - scale.x;
    float posY_u = pos.y + (scale.y / 2);
    float posY_d = posY_u - scale.y;

    if (!(point_wp.x <= posX_r && point_wp.x >= posX_l)) return false;
    if (!(point_wp.y <= posY_u && point_wp.y >= posY_d)) return false;
    return true;

}
// TODO : some type of DrawData system, where everything that wants to be drawn sends draw data to a main Draw function, to abstrac this type of code.
// this draw code does suck yes i know but its actually somewhat efficient right now

bool Sprite::draw() {
    if (m_material->m_shader == nullptr) {
        std::cout << "WARNING : material " << &m_material << " does not have a shader! Trying to apply an existing shader..." << std::endl;
        m_material->m_shader = ShaderManager::get().s_shaderList[0];
        if (m_material->m_shader == nullptr) {
            std::cout << "WARNING : Automatic shader assignment failed. No shaders exist yet, or something has gone horribly wrong." << std::endl;
            return false;
        }
        std::cout << "Successfully assigned shader!" << std::endl;
    }
    if (Camera::mainCamera != nullptr) {
        Camera::mainCamera->calculateVPMatrix();
        Camera::mainCamera->bindFramebuffer();
        m_material->m_shader->setMat4(Camera::mainCamera->vpMatrix(), vpUniform);
    }
    else {
        std::cout << "WARNING : No camera exists in the scene!" << std::endl;
    }
    m_material->m_shader->setMat4(transform.worldOffsetMatrix(), modelUniform);
    m_material->m_shader->setVec4(m_material->m_colour, "multiplyCol");
    m_material->m_shader->use();
    m_material->m_blending.use();
    m_mesh->bind();
    if (m_material->m_diffuseTexture == nullptr) {
        //std::cout << "WARNING : No texture applied to material " << &m_material << std::endl;
        m_material->m_shader->setBool(false, texAppliedUniform);
    }
    else {
        if (!m_material->m_diffuseTexture->isInitialised()) {
            m_material->m_shader->setBool(false, texAppliedUniform);
        }
        else {
            glBindTexture(m_material->m_diffuseTexture->getType(), m_material->m_diffuseTexture->getID());
            m_material->m_shader->setBool(true, texAppliedUniform);
        }
    }
    if (m_selected) {
        m_material->m_shader->setVec4(glm::vec4(
                                        Shader::m_highlightCol[0],
                                        Shader::m_highlightCol[1],
                                        Shader::m_highlightCol[2],
                                        Shader::m_highlightCol[3]
                                    ), highlightColUniform);
        m_material->m_shader->setBool(true, highlightUniform);
    }
    glDrawElements(m_mesh->m_topology, m_mesh->m_indexData.size(), GL_UNSIGNED_INT, 0);
    if (m_selected) {
        m_material->m_shader->setBool(false, highlightUniform);
    }
    if (m_wireframe || m_selected) {
        m_material->m_shader->setBool(true, wireframeUniform);
        if (m_selected) {
            m_material->m_shader->setVec4(glm::vec4(
                                            Shader::m_highlightCol[0],
                                            Shader::m_highlightCol[1],
                                            Shader::m_highlightCol[2],
                                            Shader::m_highlightCol[3]
                                        ), wireframeColUniform);
        }
        else {
            m_material->m_shader->setVec4(glm::vec4(
                                            Shader::m_wireframeCol[0],
                                            Shader::m_wireframeCol[1],
                                            Shader::m_wireframeCol[2],
                                            Shader::m_wireframeCol[3]
                                        ), wireframeColUniform);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(m_mesh->m_topology, m_mesh->m_indexData.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        m_material->m_shader->setBool(false, wireframeUniform);
    }
    //std::cout << m_entityName << ", " << WindowManager::get()->currentFrame() << std::endl;
    return true;
}

void Sprite::setScaleToTexelSize() {
    if (m_material->m_diffuseTexture == nullptr) {
        std::cout << "ERROR : No texture applied! Cannot set size to texel size from no texture stupid!" << std::endl;
        return;
    }
    glm::mat4* offset = transform.offsetMatrixPtr();
    *offset = glm::scale(
        glm::mat4(1),
        glm::vec3(
        m_material->m_diffuseTexture->dimensions().x,
        m_material->m_diffuseTexture->dimensions().y,
        1)
    );
    transform.setGlobalScale({1, 1, 1});
    transform.forceDirty();
}

void Sprite::create(std::string name) {
    if (m_initialised) return;
    ScriptableEntity::create(name);
    RenderEntity::s_allRenderEntities.push_back(this);
    oncreate_positionZ();
    if (m_material->m_diffuseTexture == nullptr)
        transform.setGlobalScale(32);
    else
        setScaleToTexelSize();
    //setScaleToTexelSize();
}

glm::vec2 Sprite::getTL() {
    return glm::vec2(length(transform.globalUp()) / 2.0f, length(transform.globalLeft()) / 2.0f);
}

glm::vec2 Sprite::getTR() {
    return glm::vec2(length(transform.globalUp()) / 2.0f, length(transform.globalRight()) / 2.0f);
}
glm::vec2 Sprite::getBL() {
    return glm::vec2(length(transform.globalDown()) / 2.0f, length(transform.globalLeft()) / 2.0f);
}
glm::vec2 Sprite::getBR() {
    return glm::vec2(length(transform.globalUp()) / 2.0f, length(transform.globalLeft()) / 2.0f);
}

glm::vec2 Sprite::getN() {
    return glm::vec2(0, (length(transform.globalUp() * trueScaleGlobal().y) / 2.0f) * ((transform.getGlobalScale().y > 0) ? 1.0f : -1.0f)) + glm::vec2(transform.getGlobalPosition().x, transform.getGlobalPosition().y);
}
glm::vec2 Sprite::getE() {
    return glm::vec2(((length(transform.globalUp() * trueScaleGlobal().x) / 2.0f) * ((transform.getGlobalScale().x > 0) ? 1.0f : -1.0f)), 0) + glm::vec2(transform.getGlobalPosition().x, transform.getGlobalPosition().y);
}
glm::vec2 Sprite::getS() {
    return glm::vec2(0, (-length(transform.globalUp() * trueScaleGlobal().y) / 2.0f) * ((transform.getGlobalScale().y > 0) ? 1.0f : -1.0f)) + glm::vec2(transform.getGlobalPosition().x, transform.getGlobalPosition().y);
}
glm::vec2 Sprite::getW() {
    return glm::vec2(-((length(transform.globalUp() * trueScaleGlobal().x) / 2.0f) * ((transform.getGlobalScale().x > 0) ? 1.0f : -1.0f)), 0) + glm::vec2(transform.getGlobalPosition().x, transform.getGlobalPosition().y);
}

CEREAL_REGISTER_TYPE(Sprite)
