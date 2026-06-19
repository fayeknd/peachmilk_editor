#include "camera.hpp"
#include "../windowmanager/WindowManager.hpp"

void Camera::calculateVPMatrix() {

    glm::vec3 euler = transform.getGlobalEulerAngles();
    euler.y += Math::pi;
    glm::vec3 forward = glm::vec3(
			glm::cos (euler.x) * glm::sin(euler.y),
			glm::sin(euler.x),
			glm::cos (euler.x) * glm::cos(euler.y)
		);
    glm::vec3 up = glm::vec3(
			glm::sin(euler.x) * glm::sin(euler.y),
			glm::cos(euler.x),
			glm::sin(euler.x) * glm::cos(euler.y)
		);
    m_viewMatrix = glm::lookAt(
        transform.getGlobalPosition(),
        transform.getGlobalPosition() + forward,
        up
    );

    float x = m_framebuffer->size().x;
    float y = m_framebuffer->size().y;

    m_ortho_size = glm::vec4(-x/2, x/2, -y/2, y/2) / m_zoomFactor;
    if (m_isOrtho)
        m_perspMatrix = glm::ortho(m_ortho_size.x, m_ortho_size.y, m_ortho_size.z, m_ortho_size.w, m_nearClip, m_farClip);
    else
        m_perspMatrix = glm::perspective(m_fov, x/y, m_nearClip, m_farClip);

    m_vpMatrix = m_perspMatrix * m_viewMatrix;
}

void Camera::bindFramebuffer() { 
    if (!m_framebuffer) {
        Framebuffer::useDefault();
        return;
    }
    if (m_framebuffer->use())
        glClear(m_clearFlags);
}

std::vector<Camera*> Camera::s_cameras {};
Camera* Camera::mainCamera = new Camera; 