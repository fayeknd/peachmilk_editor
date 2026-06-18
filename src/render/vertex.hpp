#pragma once
#include "../headers.h"

struct Vertex {
    Vertex(glm::vec3 pos) {m_position = pos;}
    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec3 uv, glm::vec3 c) {
        m_position = pos;
        m_normal = norm;
        m_uv = uv; 
        m_col = c;
    }
    Vertex(){};
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_uv; 
    glm::vec3 m_col;
};