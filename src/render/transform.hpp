#pragma once
#include "../headers.h"
#include "../system/math.hpp"
#include "../system/generic.h"
#include "../system/vechelper.hpp"

using namespace vecmath;

class Transform {
protected:

    glm::vec3 m_localPosition = glm::vec3(0);
    glm::vec3 m_localScale = glm::vec3(1);
    glm::vec3 m_localRotation = glm::vec3(0);
    glm::quat m_localRotationQ;

    glm::vec3 m_globalPosition = glm::vec3(0);
    glm::vec3 m_globalScale = glm::vec3(1);
    glm::vec3 m_globalRotation = glm::vec3(0);
    glm::quat m_globalRotationQ;

    bool m_dirty = true;
    bool m_gDirty = false;
    bool m_zDirty = true;

    glm::mat4 m_offset = glm::mat4(1);
    glm::mat4 m_worldMatrixOffset = glm::mat4(1);
    glm::mat4 m_worldMatrix = glm::mat4(1);
    glm::mat4 m_localWorldMatrix = glm::mat4(1);

    glm::vec3 b1;
    glm::vec4 b2; // needed to fill out glm::decompose TRUST me
    // like genuinely idk why null or nullptr dont just work

    bool m_lockPosToInt = false;
    bool m_lockRotToInt = false;
    bool m_locScalToInt = false;

public:

    template <class Archive>
    void serialize(Archive & archive) {
        archive(
            m_localPosition.x, m_localPosition.y, m_localPosition.z,
            m_localScale.x, m_localScale.y, m_localScale.z,
            m_localRotation.x, m_localRotation.y, m_localRotation.z,
            m_globalPosition.x, m_globalPosition.y, m_globalPosition.z,
            m_globalScale.x, m_globalScale.y, m_globalScale.z,
            m_globalRotation.x, m_globalRotation.y, m_globalRotation.z,
            m_offset[0][0], m_offset[0][1], m_offset[0][2], m_offset[0][3],
            m_offset[1][0], m_offset[1][1], m_offset[1][2], m_offset[1][3],
            m_offset[2][0], m_offset[2][1], m_offset[2][2], m_offset[2][3],
            m_offset[3][0], m_offset[3][1], m_offset[3][2], m_offset[3][3],
            m_zDirty, m_lockPosToInt, m_lockRotToInt, m_locScalToInt);
    }

    bool isDirty() { return (m_dirty || m_gDirty); }
    void forceDirty() { m_gDirty = true; m_dirty = true; }

    void calculateWorldMatrix(bool isSprite = false, glm::mat4 multiplyMatrix = glm::mat4(1), bool force = false);

    // worldOffsetMatrix is unique to things like sprites or billboards.
    // the offset matrix can allow for warping of the mesh on the individual
    // objects without affecting the children. For example, this can be used
    // to properly size a sprite with the size of (1, 1) to equal it's
    // texture dimensions.
    //
    // (a 240x460 sprite would be sized correctly per pixel at a scale of 1,1)

    glm::mat4* offsetMatrixPtr() { return &m_offset; }
    glm::mat4 worldOffsetMatrix() { return m_worldMatrixOffset; }
    glm::mat4 worldMatrix() { return m_worldMatrix; }
    glm::mat4 localMatrix() { return m_localWorldMatrix; }

    glm::vec3 getLocalEulerAngles();
    glm::vec3 getGlobalEulerAngles();

    glm::vec3 localForward();
	glm::vec3 localLeft();
	glm::vec3 localUp();
    glm::vec3 localBack() { return -localForward(); }
    glm::vec3 localRight() { return -localLeft(); }
    glm::vec3 localDown() { return -localUp(); }

    glm::vec3 globalForward();
	glm::vec3 globalLeft();
	glm::vec3 globalUp();
    glm::vec3 globalBack() { return -globalForward(); }
    glm::vec3 globalRight() { return -globalLeft(); }
    glm::vec3 globalDown() { return -globalUp(); }

    bool isScaleRounded() { return m_locScalToInt; }
    bool isRotRounded() { return m_lockRotToInt; }
    bool isPosRounded() { return m_lockPosToInt; }

    void setScaleRounded(bool val);
    void setRotRounded(bool val);
    void setPosRounded(bool val);

    glm::vec3 getLocalPosition() { return m_localPosition; }
    glm::vec3 getGlobalPosition() { return m_globalPosition; }

    void setLocalPosition(glm::vec3 pos);
    void setLocalPositionX(float x);
    void setLocalPositionY(float y);
    void setLocalPositionZ(float z);
    void translateLocalPosition(glm::vec3 pos);
    void translateLocalPositionX(float x);
    void translateLocalPositionY(float y);
    void translateLocalPositionZ(float z);

    void setGlobalPosition(glm::vec3 pos);
    void setGlobalPositionX(float x);
    void setGlobalPositionY(float y);
    void setGlobalPositionZ(float z);
    void translateGlobalPosition(glm::vec3 pos);
    void translateGlobalPositionX(float x);
    void translateGlobalPositionY(float y);
    void translateGlobalPositionZ(float z);

    glm::vec3 getLocalScale() { return m_localScale; }
    glm::vec3 getGlobalScale() { return m_globalScale; }

    void setLocalScale(glm::vec3 scale);
    void setLocalScale(float scale);
    void setLocalScaleX(float x);
    void setLocalScaleY(float y);
    void setLocalScaleZ(float z);

    void setGlobalScale(glm::vec3 scale);
    void setGlobalScale(float scale);
    void setGlobalScaleX(float x);
    void setGlobalScaleY(float y);
    void setGlobalScaleZ(float z);

    glm::vec3 getLocalRotation() { return m_localRotation; }
    glm::quat getLocalRotationQ() { return m_localRotationQ; }
    glm::vec3 getGlobalRotation() { return m_globalRotation; }
    glm::quat getGlobalRotationQ() { return m_globalRotationQ; }

    void setLocalRotation(glm::vec3 rot);
    void setLocalRotationX(float x);
    void setLocalRotationY(float y);
    void setLocalRotationZ(float z);

    void setGlobalRotation(glm::vec3 rot);
    void setGlobalRotationX(float x);
    void setGlobalRotationY(float y);
    void setGlobalRotationZ(float z);

    bool checkCollisionAgainstPoint(glm::vec2 point, bool local = false);
    bool checkCollisionAgainstTransform(Transform* t);

};
