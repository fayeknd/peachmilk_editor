#pragma once
#include "../headers.h"
#include "entity.hpp"
#include "framebuffer.hpp"

class Camera : public ScriptableEntity {
protected:

    glm::mat4 m_perspMatrix = glm::mat4(1);
    glm::mat4 m_viewMatrix = glm::mat4(1);
    
    glm::mat4 m_vpMatrix;

    static std::vector<Camera*> s_cameras;

public:

    static Camera* mainCamera;
    Framebuffer* m_framebuffer = nullptr;

    void bindFramebuffer();
    void calculateVPMatrix();
    glm::mat4 perspMatrix() { return m_perspMatrix; }
    glm::mat4 viewMatrix() { return m_viewMatrix; }
    glm::mat4 vpMatrix() { return m_vpMatrix; }
    static std::vector<Camera*> allCameras() { return s_cameras; };

    float m_fov = 75;
    float m_nearClip = 0;
    float m_farClip = 1000.0f;
    float m_zoomFactorDefault = 10.0f;
    float m_zoomFactor = m_zoomFactorDefault;
    float m_zoomFactorMin = 0.001f;
    float m_zoomFactorMax = 10000.0f;

    bool m_isOrtho = true;
    
    glm::vec4 m_ortho_size;

    GLbitfield m_clearFlags = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera() { s_cameras.push_back(this); }
    ~Camera() { 
        for (int i = 0; i < s_cameras.size(); i++) {
            if (s_cameras[i] == this) {
                s_cameras.erase(s_cameras.begin() + i);
                break;
            }
        }
    }

};