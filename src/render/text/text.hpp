#pragma once
#include "../../headers.h"
#include "../entity.hpp"
#include "../material.hpp"
#include "../mesh.hpp"
#include "font.hpp"

enum TextAlignment {
    Center = 0,
    Left = 1,
    Right = 2
};

class TextRenderer : public ScriptableEntity, public RenderEntity{
private:

    std::string m_text = "Empty Text";
    bool m_isDirty = true;
    Mesh m_mesh;
    unsigned int m_fontID;
    float m_lineHeight;
    TextAlignment m_textAlignment = TextAlignment::Left;
    glm::mat4 offset;
    bool m_copyToChildren;
    TextFont* m_font = nullptr;
public:

    void setFont(TextFont* font);
    TextFont* getFont() { return m_font; }

    void create(std::string name = "TextRenderer ") override;
    ~TextRenderer();
    template <class Archive>
    void serialize(Archive & archive) {
        m_fontID = (m_font) ? m_font->getID() : -1;
        auto tAlign = static_cast<int>(m_textAlignment);
        archive(cereal::base_class<ScriptableEntity>(this), m_text, m_fontID, tAlign, m_colourMultiplier.x, m_colourMultiplier.y, m_colourMultiplier.z, m_colourMultiplier.w, m_copyToChildren);
        m_textAlignment = static_cast<TextAlignment>(tAlign);
    }
    void setCopyToChildren(bool copy);
    bool getCopyToChildren() { return m_copyToChildren; };
    void setTextAlignment(TextAlignment alignment) { m_textAlignment = alignment; m_isDirty = true; }
    TextAlignment getTextAlignment() { return m_textAlignment; }
    std::string getText() { return m_text; }
    glm::vec4 m_colourMultiplier = glm::vec4(1);
    void setText(std::string text);
    float getFontLineSpacing() { return m_font->m_lineSpacing; }
    void setFontLineSpacing(float spacing);
    void _deserializeFnc() override;
    void packTextMesh();
    bool draw() override;

};
