#pragma once
#include "../headers.h"
#include "vertex.hpp"
#include <vector>

struct Mesh {

    static Mesh Quad;
    static void s_createUniqueMeshes();
    static std::vector<Mesh*> s_meshes; 
    static unsigned int s_boundMesh;

    std::vector<Vertex> m_vertexData;
    std::vector<unsigned int> m_indexData;
    
    Mesh(std::vector<Vertex> vertexData, std::vector<unsigned int> indexData, int topology = GL_TRIANGLES) {
        setupMesh(vertexData, indexData, topology);
    }
    Mesh();
    ~Mesh() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
        for (int i = 0; i < s_meshes.size(); i++) {
        if (s_meshes[i] == this) {
            s_meshes.erase(s_meshes.begin() + i);
            return;
        }
    }
    }
    
    void setupMesh(std::vector<Vertex> vertexData, std::vector<unsigned int> indexData, int topology = GL_TRIANGLES);

    void bind();

    unsigned int getVBO() { return m_vbo; }
    unsigned int getVAO() { return m_vao; }

    int m_topology;

private:
    unsigned int m_vbo;
    unsigned int m_vao;
    unsigned int m_ebo;
    bool init = false;
    static inline bool s_uniqueMeshesInit = false;
};