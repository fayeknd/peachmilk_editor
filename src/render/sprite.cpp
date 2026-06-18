#include "sprite.hpp"
#include "camera.hpp"
#include "../render/camera.hpp"
#include "entity.hpp"

Sprite::~Sprite() {
    for (int i = 0; i < s_sprites.size(); i++) {
        if (s_sprites[i] == this) {
            s_sprites.erase(s_sprites.begin() + i);
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
    glm::vec3 point_wp = glm::vec3(point.x, point.y, 0) + Camera::mainCamera->transform.getLocalPosition();
    // right, left, up, down
    float posX_r = pos.x + (scale.x / 2);
    float posX_l = posX_r - scale.x;
    float posY_u = pos.y + (scale.y / 2);
    float posY_d = posY_u - scale.y;

    if (!(point_wp.x <= posX_r && point_wp.x >= posX_l)) return false;
    if (!(point_wp.y <= posY_u && point_wp.y >= posY_d)) return false;
    return true;

}

void Sprite::orderSprites(bool flip) {
    quicksort(0, s_sprites.size() - 1, flip);
}

float Sprite::partition(int left, int right, bool flip) {
    float pivot = s_sprites[right]->transform.getGlobalPosition().z;
    int sortedIndex = left - 1;

    for (int j = left; j < right; j++) {
        if (flip) {

            if (s_sprites[j]->transform.getGlobalPosition().z > pivot) {
                sortedIndex++;
                std::swap(s_sprites[sortedIndex], s_sprites[j]);
            }
        }
        else if (s_sprites[j]->transform.getGlobalPosition().z < pivot) {
            sortedIndex++;
            std::swap(s_sprites[sortedIndex], s_sprites[j]);
        }
    }

    std::swap(s_sprites[sortedIndex + 1], s_sprites[right]);
    return sortedIndex + 1;
}

void Sprite::quicksort(int left, int right, bool flip) {
    if (left < right) {
        int pivot = partition(left, right, flip);

        quicksort(left, pivot - 1, flip);
        quicksort(pivot + 1, right, flip);
    }
}

// TODO : some type of DrawData system, where everything that wants to be drawn sends draw data to a main Draw function, to abstrac this type of code.
// this draw code does suck yes i know but its actually somewhat efficient right now

bool Sprite::draw() {
    if (!ScriptableEntity::draw()) return false;
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
    s_sprites.push_back(this);
    if (s_sprites.size() == 1) {
        transform.setGlobalPositionZ(-255);
    }
    else {
        transform.setGlobalPositionZ(s_sprites[s_sprites.size() - 2]->transform.getGlobalPosition().z + 1);
    }
    if (m_material->m_diffuseTexture == nullptr) 
        transform.setGlobalScale(32);
    else
        setScaleToTexelSize();
    //setScaleToTexelSize();
}

CEREAL_REGISTER_TYPE(Sprite);