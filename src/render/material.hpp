#pragma once
#include "../shader/ShaderManager.hpp"
#include "texture.hpp"
#include "rendertexture.hpp"
#include "../headers.h"
#include <iostream>
#include "blend.hpp"
#include "../editor/project.hpp"

class Material {
private:

    unsigned int m_ID;
    static inline unsigned int s_availableID = 0;
    static inline unsigned int s_largestIDused = 0;
    std::string m_deserializedTexPath = "";
    unsigned int m_deserializedShaderID;
    std::string m_serializedPath = "";

    glm::ivec2 m_uvOffset {0};

public:

    template<class Archive>
    void serialize(Archive & archive)
    {
        m_deserializedTexPath = (m_diffuseTexture != nullptr) ? m_diffuseTexture->getFilePath() : "";
        m_deserializedShaderID = (m_shader != nullptr) ? m_shader->getShaderID() : -1;

        archive(m_colour.x, m_colour.y, m_colour.z, m_colour.w, m_blending.m_mode, m_deserializedTexPath, m_ID, m_serializeTex); // serialize things by passing them to the archive
    }

    bool m_serialize = true;
    bool m_serializeTex = true;
    static void deseralizeMaterials(std::string cache = "");
    static void serializeMaterials(std::string cache = "");
    std::string getDeserializedTexPath() { return m_deserializedTexPath; }
    unsigned int getDeserializedShaderID() { return m_deserializedShaderID; }
    unsigned int getID() { return m_ID; }
    static Material* getMaterialViaID(unsigned int id);

    static std::vector<Material*> s_allMaterials;
    static Material* s_defaultMaterial;

    Material() {
        s_allMaterials.push_back(this);
        if (s_availableID <= s_largestIDused) s_availableID = s_largestIDused + 1;
        m_ID = s_availableID;
        s_largestIDused = s_availableID;
        s_availableID++;
    }
    ~Material() {
        for (int i = 0; i < s_allMaterials.size(); i++) {
            if (s_allMaterials[i] == this) {
                s_allMaterials.erase(s_allMaterials.begin() + i);
                break;
            }
        }
    }

    void destroy() {
        std::remove(m_serializedPath.c_str());
        delete this;
    }

    BlendMode m_blending;
    glm::vec4 m_colour = glm::vec4(1);
    Texture* m_diffuseTexture = nullptr;
    Shader* m_shader = nullptr;

    RenderTexture m_displayTexture;
    bool m_displayTextureRendered = false;

    friend class TextFont;

};
