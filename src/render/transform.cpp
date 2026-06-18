#include "transform.hpp"
#include "sprite.hpp"
#include "../system/mouse.hpp"
#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_impl_glfw.h"
#include "../../include/imgui/imgui_impl_opengl3.h"
#include "../render/camera.hpp"
#include "entity.hpp"

bool Transform::checkCollisionAgainstTransform(Transform* t) {
    glm::vec3 pos1 = m_globalPosition;
    glm::vec3 pos2 = t->m_globalPosition;
    glm::vec3 size1 = m_globalScale;
    glm::vec3 size2 = t->m_globalScale;

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

bool Transform::checkCollisionAgainstPoint(glm::vec2 point, bool local) {
    glm::vec3 pos, scale;
    if (!local) {
        pos = m_globalPosition;
        scale = m_globalScale; 
    }
    else {
        pos = m_localPosition;
        scale = m_localScale;
    }
    point = (point / Camera::mainCamera->m_zoomFactor);
    // point_worldPosition
    glm::vec3 point_wp = glm::vec3(point.x, point.y, 0) + Camera::mainCamera->transform.m_localPosition;
    // right, left, up, down
    float posX_r = pos.x + (scale.x / 2);
    float posX_l = posX_r - scale.x;
    float posY_u = pos.y + (scale.y / 2);
    float posY_d = posY_u - scale.y;

    if (!(point_wp.x <= posX_r && point_wp.x >= posX_l)) return false;
    if (!(point_wp.y <= posY_u && point_wp.y >= posY_d)) return false;
    return true;

}

void Transform::calculateWorldMatrix(bool isSprite, glm::mat4 multiplyMatrix, bool force) {
    if (m_dirty || force) {

        //std::cout << m_localPosition.x << std::endl; // for testing how often this function runs
        if (!m_gDirty) {

            glm::mat4 trans = glm::translate(glm::mat4(1), m_localPosition);

            trans = glm::rotate(trans, (float)Math::deg2rad(m_localRotation.y), Math::up());
            trans = glm::rotate(trans, (float)Math::deg2rad(m_localRotation.x), Math::right());
            trans = glm::rotate(trans, (float)Math::deg2rad(m_localRotation.z), Math::back());

            m_localWorldMatrix = glm::scale(trans, m_localScale);
            m_worldMatrix = multiplyMatrix * m_localWorldMatrix;

            // this function is not a traditional function but rather holy in nature and has guided me to god 
            glm::decompose(m_worldMatrix, m_globalScale, m_globalRotationQ, m_globalPosition, b1, b2); 
            m_globalRotation = glm::eulerAngles(m_globalRotationQ);
            m_globalRotation = glm::vec3(
                Math::rad2deg(m_globalRotation.x),
                Math::rad2deg(m_globalRotation.y),
                Math::rad2deg(-m_globalRotation.z));

            //std::cout << m_globalRotation.z << std::endl;
        }
        else {

            glm::mat4 trans = glm::translate(glm::mat4(1), m_globalPosition);

            trans = glm::rotate(trans, (float)Math::deg2rad(m_globalRotation.y), Math::up());
            trans = glm::rotate(trans, (float)Math::deg2rad(m_globalRotation.x), Math::right());
            trans = glm::rotate(trans, (float)Math::deg2rad(m_globalRotation.z), Math::back());

            m_worldMatrix = glm::scale(trans, m_globalScale);
            m_localWorldMatrix = glm::inverse(multiplyMatrix) * m_worldMatrix;

            glm::decompose(m_localWorldMatrix, m_localScale, m_localRotationQ, m_localPosition, b1, b2); 
            m_localRotation = glm::eulerAngles(m_localRotationQ);
            m_localRotation = glm::vec3(
                Math::rad2deg(m_localRotation.x),
                Math::rad2deg(m_localRotation.y),
                Math::rad2deg(-m_localRotation.z));

            m_gDirty = false;
        }
        m_worldMatrixOffset = m_worldMatrix * m_offset;
        m_dirty = false;
        if (m_zDirty && isSprite) {
            std::cout << "Reordering sprites.." << std::endl;
            m_zDirty = false;
            Sprite::orderSprites(true);
        }
    }
}

glm::vec3 Transform::getLocalEulerAngles() {
    return glm::vec3(Math::deg2rad(m_localRotation.x), Math::deg2rad(m_localRotation.y), Math::deg2rad(m_localRotation.z));
}

glm::vec3 Transform::getGlobalEulerAngles() {
    return glm::vec3(Math::deg2rad(m_globalRotation.x), Math::deg2rad(m_globalRotation.y), Math::deg2rad(m_globalRotation.z));
}

glm::vec3 Transform::localForward() {
    glm::vec3 euler = getLocalEulerAngles();
    return glm::vec3(
        glm::cos(euler.x) * glm::sin(euler.y),
        glm::sin(euler.x),
        glm::cos(euler.x) * glm::cos(euler.y)
    );
}

glm::vec3 Transform::localLeft() {
    glm::vec3 euler = getLocalEulerAngles();
    return glm::vec3(
        glm::cos(euler.y),
        0,
       -glm::sin(euler.y)
    );
}

glm::vec3 Transform::localUp() {
    glm::vec3 euler = getLocalEulerAngles();
    return glm::vec3(
        glm::sin(euler.x) * glm::sin(euler.y),
        glm::cos(euler.x),
        glm::sin(euler.x) * glm::cos(euler.y)
    );
}

glm::vec3 Transform::globalForward() {
    glm::vec3 euler = getGlobalEulerAngles();
    return glm::vec3(
        glm::cos(euler.x) * glm::sin(euler.y),
        glm::sin(euler.x),
        glm::cos(euler.x) * glm::cos(euler.y)
    );
}

glm::vec3 Transform::globalLeft() {
    glm::vec3 euler = getGlobalEulerAngles();
    return glm::vec3(
        glm::cos(euler.y),
        0,
       -glm::sin(euler.y)
    );
}

glm::vec3 Transform::globalUp() {
    glm::vec3 euler = getGlobalEulerAngles();
    return glm::vec3(
        glm::sin(euler.x) * glm::sin(euler.y),
        glm::cos(euler.x),
        glm::sin(euler.x) * glm::cos(euler.y)
    );
}

void Transform::setLocalPosition(glm::vec3 pos) { 
    if (pos.z != m_localPosition.z) m_zDirty = true; 
    m_localPosition = pos;
    m_dirty = true;
}

void Transform::setGlobalPosition(glm::vec3 pos) { 
    if (pos.z != m_globalPosition.z) m_zDirty = true; 
    m_globalPosition = pos;
    m_gDirty = true;
}


void Transform::setLocalPositionX(float x) { 
    m_localPosition.x = x; 
    m_dirty = true;
}
void Transform::setLocalPositionY(float y) { 
    m_localPosition.y = y; 
    m_dirty = true;
}
void Transform::setLocalPositionZ(float z) { 
    if (m_localPosition.z != z) m_dirty = true;
    m_localPosition.z = z; 
    m_dirty = true;
}

void Transform::setGlobalPositionX(float x) { 
    m_globalPosition.x = x; 
    m_gDirty = true;
}
void Transform::setGlobalPositionY(float y) { 
    m_globalPosition.y = y; 
    m_gDirty = true;
}
void Transform::setGlobalPositionZ(float z) { 
    if (m_globalPosition.z != z) m_dirty = true;
    m_globalPosition.z = z; 
    m_gDirty = true;
}



void Transform::setLocalScale(glm::vec3 scale) { 
    m_localScale = scale;
    m_dirty = true;
}

void Transform::setGlobalScale(glm::vec3 scale) { 
    m_globalScale = scale;
    m_gDirty = true;
}


void Transform::setLocalScale(float scale) {
    m_localScale.x = scale;
    m_localScale.y = scale;
    m_localScale.z = scale;
    m_dirty = true;
}

void Transform::setGlobalScale(float scale) {
    m_globalScale.x = scale;
    m_globalScale.y = scale;
    m_globalScale.z = scale;
    m_gDirty = true;
}

void Transform::setLocalScaleX(float x) { 
    m_localScale.x = x;
    m_dirty = true;
}
void Transform::setLocalScaleY(float y) { 
    m_localScale.y = y;
    m_dirty = true;
}
void Transform::setLocalScaleZ(float z) { 
    m_localScale.z = z;
    m_dirty = true;
}

void Transform::setGlobalScaleX(float x) { 
    m_globalScale.x = x;
    m_gDirty = true;
}
void Transform::setGlobalScaleY(float y) { 
    m_globalScale.y = y;
    m_gDirty = true;
}
void Transform::setGlobalScaleZ(float z) { 
    m_globalScale.z = z;
    m_gDirty = true;
}



void Transform::setLocalRotation(glm::vec3 rot) { 
    m_localRotation = rot;
    m_dirty = true;
}

void Transform::setGlobalRotation(glm::vec3 rot) { 
    m_globalRotation = rot;
    m_gDirty = true;
}

void Transform::setLocalRotationX(float x) { 
    m_localRotation.x = x;
    m_dirty = true;
}
void Transform::setLocalRotationY(float y) { 
    m_localRotation.y = y;
    m_dirty = true;
}
void Transform::setLocalRotationZ(float z) { 
    m_localRotation.z = z;
    m_dirty = true;
}

void Transform::setGlobalRotationX(float x) { 
    m_globalRotation.x = x;
    m_gDirty = true;
}
void Transform::setGlobalRotationY(float y) { 
    m_globalRotation.y = y;
    m_gDirty = true;
}
void Transform::setGlobalRotationZ(float z) { 
    m_globalRotation.z = z;
    m_gDirty = true;
}
    

    
