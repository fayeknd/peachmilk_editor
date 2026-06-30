#include "mesh.hpp"
#include <iostream>

Mesh Mesh::Quad;

Mesh::Mesh() {};

std::vector<Mesh*> Mesh::s_meshes;

unsigned int Mesh::s_boundMesh = -1;

void Mesh::bind() {
    if (s_boundMesh == m_vao)
        return;
    glBindVertexArray(m_vao);
    s_boundMesh = m_vao;
}

void Mesh::s_createUniqueMeshes() {

    if (s_uniqueMeshesInit)
        return;
    // Quad

    Vertex v0, v1, v2, v3;
    v0.m_position = glm::vec3( 0.5f,  0.5f, 0.0f);
    v1.m_position = glm::vec3( 0.5f, -0.5f, 0.0f);
    v2.m_position = glm::vec3(-0.5f, -0.5f, 0.0f);
    v3.m_position = glm::vec3(-0.5f,  0.5f, 0.0f);

    v0.m_uv = glm::vec2(1, 1);
    v1.m_uv = glm::vec2(1, 0);
    v2.m_uv = glm::vec2(0, 0);
    v3.m_uv = glm::vec2(0, 1);

    std::vector<Vertex> vertices {
        v0, v1, v2, v3
    };

    std::vector<unsigned int> indices {
        0, 1, 3, 1, 2, 3
    };

    Quad.setupMesh(vertices, indices, GL_TRIANGLES);

    s_uniqueMeshesInit = true;
}

void Mesh::setupMesh(std::vector<Vertex> vertexData, std::vector<unsigned int> indexData, int topology, int mode) {

    if (init)
        return;

    s_meshes.push_back(this);

    m_vertexData = vertexData;
    m_indexData = indexData;
    m_topology = topology;

    size_t vertexSize = sizeof(Vertex);

    // god i love writing opengl code and rawdogging instructions with no indentation

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * vertexSize, &m_vertexData[0], mode);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size() * sizeof(unsigned int), &m_indexData[0], mode);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(Vertex, m_position));
    glEnableVertexAttribArray(0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(Vertex, m_normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(Vertex, m_col));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)offsetof(Vertex, m_uv));

    glBindVertexArray(0);
    init = true;
}
