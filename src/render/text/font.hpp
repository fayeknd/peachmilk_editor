#pragma once
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include "../../headers.h"
#include "../material.hpp"
#include <algorithm>

#define TEST_FACE_LOC "arial.ttf"
#define CHARS_X 16
#define CHARS_Y 16
#define MAX_CHARS std::min((CHARS_X * CHARS_Y), 255)
#define ENGINE_DEFTAULT_FONT_SIZE 128
#define ENGINE_DEFAULT_LINE_SPACING 0.8f

struct TextChar {
    glm::ivec2   m_size;
    glm::ivec2   m_bearing;
    long m_advance;
    glm::ivec2   m_uvOffset;

    template <class Archive>
    void serialize(Archive& archive) {
        archive(m_size.x, m_size.y, m_bearing.x, m_bearing.y, m_advance, m_uvOffset.x, m_uvOffset.y);
    }
};


class TextFont {
private:

    unsigned int m_ID;
    static inline unsigned int s_availableID = 0;
    static inline unsigned int s_largestIDused = 0;

    std::string m_fontLoc = std::string("");
    FT_Library m_ftLib;
    bool m_fontLoaded = false;
    std::map<char, TextChar> m_characters;
    Material* m_material = Material::s_defaultMaterial;
    unsigned int m_matID;
    FT_Face m_face;
    bool loadChars(unsigned int charSize = ENGINE_DEFTAULT_FONT_SIZE);
    unsigned int m_charSize;
    unsigned int m_filterMode = GL_LINEAR;
    // so sure this has a name, but I couldn't find it so this is what it's called now
    float m_lineSpacing = ENGINE_DEFAULT_LINE_SPACING;

public:
    TextFont() = default;
    ~TextFont();

    static inline std::vector<TextFont*> s_textFonts {};

    bool m_serialize = true;

    std::string getFontLoc() { return m_fontLoc; }
    bool loadTypeFace(std::string path, unsigned int charSize = -1);
    static TextFont* loadFont(std::string path, unsigned int charSize);
    static void serializeFonts(std::string cache = "");
    static void deserializeFonts(std::string cache = "");
    static TextFont* getFontViaID(unsigned int id);
    static TextFont* getFontViaTTFFile(std::string loc);
    unsigned int getID() { return m_ID; }

    template <class Archive>
    void serialize(Archive & archive) {
        if (m_material) {
            if (m_material->m_diffuseTexture)
                if (m_material->m_diffuseTexture->isInitialised())
                    m_filterMode = m_material->m_diffuseTexture->getFilterMode();
            m_matID = m_material->getID();
        }
        else m_matID = -1;
        archive(m_matID, m_ID, m_characters, m_charSize, m_fontLoc, m_filterMode, m_lineSpacing);
    }
    friend class TextRenderer;

};
