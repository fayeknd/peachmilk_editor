#include "text.hpp"

// FreeType functions return a non-zero value when there is an error.

void TextRenderer::create(std::string name) {
    if (m_initialised) return;
    ScriptableEntity::create(name);
    RenderEntity::s_allRenderEntities.push_back(this);
    oncreate_positionZ();
    m_mesh.setupMesh(Mesh::Quad.m_vertexData, Mesh::Quad.m_indexData, Mesh::Quad.m_topology, GL_DYNAMIC_DRAW);
}
TextRenderer::~TextRenderer() {
    for (int i = 0; i < RenderEntity::s_allRenderEntities.size(); i++) {
        if (RenderEntity::s_allRenderEntities[i] == this) {
            RenderEntity::s_allRenderEntities.erase(RenderEntity::s_allRenderEntities.begin() + i);
            break;
        }
    }
}

void TextRenderer::packTextMesh() {
    if (m_isDirty) {

        m_mesh.m_vertexData.clear();
        m_mesh.m_indexData.clear();

        std::string::const_iterator c;
        float x = 0;
        float y = 0;
        int i = 0;

        std::vector<std::pair<float, Mesh>> lines;
        char lastChar = 0;
        Mesh newMesh;
        for (c = m_text.begin(); c != m_text.end(); c++) {

            if (lastChar == '\\' && *c == 'n') {
                lines.push_back(std::pair<float, Mesh>(x, newMesh));
                y -= m_font->m_lineSpacing * static_cast<float>(m_font->m_charSize);
                x = 0;
                newMesh.m_indexData.clear();
                newMesh.m_vertexData.clear();
                lastChar = 0;
            }
            else if (*c != '\\'){
                TextChar* ch = &m_font->m_characters[*c];
                float xpos = x + ch->m_bearing.x;
                float ypos = y -(ch->m_size.y - ch->m_bearing.y);

                float w = ch->m_size.x;
                float h = ch->m_size.y;
                // update VBO for each character

                Vertex v0, v1, v2, v3;
                unsigned int i0, i1, i2, i3, i4, i5;
                v0.m_position = glm::vec3(xpos + w, ypos,       0);
                v1.m_position = glm::vec3(xpos + w, ypos + h,   0);
                v2.m_position = glm::vec3(xpos,     ypos + h,   0);
                v3.m_position = glm::vec3(xpos,     ypos,       0);

                v0.m_uv = glm::vec2(ch->m_uvOffset.x + ch->m_size.x, -(ch->m_uvOffset.y + ch->m_size.y)) / m_font->m_material->m_diffuseTexture->dimensions();
                v1.m_uv = glm::vec2(ch->m_uvOffset.x + ch->m_size.x, -(ch->m_uvOffset.y))                / m_font->m_material->m_diffuseTexture->dimensions();
                v2.m_uv = glm::vec2(ch->m_uvOffset.x,  -(ch->m_uvOffset.y))                              / m_font->m_material->m_diffuseTexture->dimensions();
                v3.m_uv = glm::vec2(ch->m_uvOffset.x,  -(ch->m_uvOffset.y + ch->m_size.y))               / m_font->m_material->m_diffuseTexture->dimensions();

                i0 = 0 + (i*4);
                i1 = 3 + (i*4);
                i2 = 2 + (i*4);
                i3 = 0 + (i*4);
                i4 = 2 + (i*4);
                i5 = 1 + (i*4);

                newMesh.m_vertexData.push_back(v0);
                newMesh.m_vertexData.push_back(v1);
                newMesh.m_vertexData.push_back(v2);
                newMesh.m_vertexData.push_back(v3);

                m_mesh.m_indexData.push_back(i0);
                m_mesh.m_indexData.push_back(i1);
                m_mesh.m_indexData.push_back(i2);
                m_mesh.m_indexData.push_back(i3);
                m_mesh.m_indexData.push_back(i4);
                m_mesh.m_indexData.push_back(i5);

                x += (ch->m_advance >> 6);
                i++;
            }
            lastChar = *c;
            if (c + 1 == m_text.end()) {
                lines.push_back(std::pair<float, Mesh>(x, newMesh));
                newMesh.m_indexData.clear();
                newMesh.m_vertexData.clear();
            }

        }

        // this can and was done with an offset matrix
        // which worked for a single line. it does not
        // work for multiple lines.
        // to be honest this is probably a much more
        // efficient way.
        if (lines.size() > 0) {
            for (int i = 0; i < lines.size(); i++) {
                for (auto vertex : lines[i].second.m_vertexData) {
                    switch (m_textAlignment) {
                        case TextAlignment::Center:
                        vertex.m_position.x -= lines[i].first / 2;
                        break;
                        case TextAlignment::Right:
                        vertex.m_position.x -= lines[i].first;
                        break;
                        default:
                        break;
                    }
                    m_mesh.m_vertexData.push_back(vertex);
                }
            }
        }
        else {
            m_mesh.m_vertexData.push_back(Vertex());
            m_mesh.m_indexData.push_back(0);
        }

        lines.clear();
        m_mesh.bind();

        glBindBuffer(GL_ARRAY_BUFFER, m_mesh.getVBO());
        glBufferData(GL_ARRAY_BUFFER, m_mesh.m_vertexData.size() * sizeof(Vertex), &m_mesh.m_vertexData[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh.getEBO());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.m_indexData.size() * sizeof(unsigned int), &m_mesh.m_indexData[0], GL_DYNAMIC_DRAW);

        m_isDirty = false;
    }
}

bool TextRenderer::draw() {
    if (m_isDirty && m_copyToChildren) {
        for (int j = 0; j < m_children.size(); j++) {
            TextRenderer* text = dynamic_cast<TextRenderer*>(m_children[j]);
            if (text) {
                text->setText(m_text);
                text->setTextAlignment(m_textAlignment);
            }
        }
    }
    if (!m_font) return false;
    if (!m_font->m_fontLoaded) return false;
    if (m_font->m_material->m_shader == nullptr) {
        std::cout << "WARNING : material " << &m_font->m_material << " does not have a shader! Trying to apply an existing shader..." << std::endl;
        m_font->m_material->m_shader = ShaderManager::get().s_shaderList[0];
        if (m_font->m_material->m_shader == nullptr) {
            std::cout << "WARNING : Automatic shader assignment failed. No shaders exist yet, or something has gone horribly wrong." << std::endl;
            return false;
        }
        std::cout << "Successfully assigned shader!" << std::endl;
    }
    m_font->m_material->m_shader->use();
    m_font->m_material->m_blending.use();
    if (Camera::mainCamera != nullptr) {
        Camera::mainCamera->calculateVPMatrix();
        Camera::mainCamera->bindFramebuffer();
        m_font->m_material->m_shader->setMat4(Camera::mainCamera->vpMatrix(), vpUniform);
    }
    else {
        std::cout << "WARNING : No camera exists in the scene!" << std::endl;
    }

    if (m_selected) {
        m_font->m_material->m_shader->setVec4(glm::vec4(
                                        Shader::m_highlightCol[0],
                                        Shader::m_highlightCol[1],
                                        Shader::m_highlightCol[2],
                                        Shader::m_highlightCol[3]
                                    ), highlightColUniform);
        m_font->m_material->m_shader->setBool(true, highlightUniform);
    }
    packTextMesh();
    // bind() will not rebind if the mesh is already bound.
    m_mesh.bind();
    //Mesh::Quad.bind(); // just for testing how the atlas looks
    m_font->m_material->m_shader->setMat4(transform.worldOffsetMatrix(), modelUniform);
    m_font->m_material->m_shader->setVec4(m_font->m_material->m_colour * m_colourMultiplier, "multiplyCol");
    m_font->m_material->m_shader->setBool(true, "drawText");
    m_font->m_material->m_shader->setBool(true, texAppliedUniform);

    glBindTexture(m_font->m_material->m_diffuseTexture->getType(), m_font->m_material->m_diffuseTexture->getID());

    glDrawElements(m_mesh.m_topology, m_mesh.m_indexData.size(), GL_UNSIGNED_INT, 0);
    if (m_wireframe || m_selected) {
            m_font->m_material->m_shader->setBool(true, wireframeUniform);
            if (m_selected) {
                m_font->m_material->m_shader->setVec4(glm::vec4(
                                                Shader::m_highlightCol[0],
                                                Shader::m_highlightCol[1],
                                                Shader::m_highlightCol[2],
                                                Shader::m_highlightCol[3]
                                            ), wireframeColUniform);
            }
            else {
                m_font->m_material->m_shader->setVec4(glm::vec4(
                                                Shader::m_wireframeCol[0],
                                                Shader::m_wireframeCol[1],
                                                Shader::m_wireframeCol[2],
                                                Shader::m_wireframeCol[3]
                                            ), wireframeColUniform);
            }
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(m_mesh.m_topology, m_mesh.m_indexData.size(), GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    if (m_selected) {
        m_font->m_material->m_shader->setBool(false, highlightUniform);
    }
    //std::cout << m_entityName << ", " << WindowManager::get()->currentFrame() << std::endl;
    m_font->m_material->m_shader->setBool(false, wireframeUniform);
    m_font->m_material->m_shader->setBool(false, "drawText");
    return true;
}

void TextRenderer::setCopyToChildren(bool copy) {
    if (m_copyToChildren == copy) return;
    m_isDirty = true;
    m_copyToChildren = copy;
}

void TextRenderer::setText(std::string text) {
    if (text == m_text) return;
    m_isDirty = true;
    m_text = text;
}

void TextRenderer::setFont(TextFont* font) {
    if (m_font == font) return;
    m_isDirty = true;
    m_font = font;
}

void TextRenderer::setFontLineSpacing(float spacing) {
    if (m_font->m_lineSpacing == spacing) return;
    m_isDirty = true;
    m_font->m_lineSpacing = spacing;
}

void TextRenderer::_deserializeFnc() {
    ScriptableEntity::_deserializeFnc();
    if (m_fontID != -1) m_font = TextFont::getFontViaID(m_fontID);
    if (!m_font) m_fontID = -1;
    m_mesh.setupMesh(Mesh::Quad.m_vertexData, Mesh::Quad.m_indexData, Mesh::Quad.m_topology, GL_DYNAMIC_DRAW);
    RenderEntity::s_allRenderEntities.push_back(this);
}

CEREAL_REGISTER_TYPE(TextRenderer)
