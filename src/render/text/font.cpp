#include "font.hpp"
#include "../textureatlas.hpp"
#include "../../editor/project.hpp"
#include <filesystem>

TextFont* TextFont::loadFont(std::string path, unsigned int charSize) {
    TextFont* f = new TextFont;
    // First check to see if an existing font
    TextFont* _loadedFont = TextFont::getFontViaTTFFile(path);
    if (_loadedFont) {
        if (charSize = -1) return _loadedFont;
        else if (_loadedFont->m_charSize == charSize) return _loadedFont;
    }
    if (charSize = -1) charSize = ENGINE_DEFTAULT_FONT_SIZE;
    if (f->loadTypeFace(path, charSize))
        return f;
    return nullptr;
}

bool TextFont::loadTypeFace(std::string path, unsigned int charSize) {
    if (m_fontLoaded) return false;
    m_charSize = charSize;
    m_fontLoc = path;

    if (FT_Init_FreeType(&m_ftLib)) {
        std::cout << "ERR : Could not init Freetype." << std::endl;
        return false;
    }
    int err = FT_New_Face(m_ftLib, path.c_str(), 0, &m_face);
    if (err == FT_Err_Unknown_File_Format) {
        std::cout << "ERR : Unknown file format or corrupted file! Error Code: " << err << std::endl;
        return false;
    }
    else if (err) {
        std::cout << "ERR : Failed to load FreeType font" << std::endl;
        return false;
    }

    if (s_availableID <= s_largestIDused) s_availableID = s_largestIDused + 1;
    m_ID = s_availableID;
    s_largestIDused = s_availableID;
    s_availableID++;

    bool success = loadChars(charSize);
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_ftLib);
    s_textFonts.push_back(this);
    std::string file = Project::get()->getFontFolder() + "\\" + std::filesystem::path(m_fontLoc).filename().string();
    if (!File::fileExists(file)) std::filesystem::copy_file(m_fontLoc, file);
    return success;
}

bool TextFont::loadChars(unsigned int charSize) {

    FT_Set_Pixel_Sizes(m_face, 0, charSize);
    FT_Select_Charmap(m_face, FT_ENCODING_UNICODE);
    std::vector<TextureData1C> tData;
    for (unsigned char i = 0; i < MAX_CHARS; i++) {
        if (FT_Load_Char(m_face, i, FT_LOAD_RENDER)) {
            std::cout << "ERR : Failed to load glyph '" << i << "'!" << std::endl;
            continue;
        }

        TextureData1C glyphTex;
        glyphTex.m_size.x  = m_face->glyph->bitmap.width;
        glyphTex.m_size.y = m_face->glyph->bitmap.rows;

        if (glyphTex.m_size.x > 0 && glyphTex.m_size.y > 0) {
            glyphTex.m_pixels.resize(glyphTex.m_size.x * glyphTex.m_size.y);
            for (int row = 0; row < glyphTex.m_size.y; ++row) {
                uint8_t* srcRowStart = m_face->glyph->bitmap.buffer + (row * m_face->glyph->bitmap.pitch);
                uint8_t* destRowStart = &glyphTex.m_pixels[row * glyphTex.m_size.x];
                std::memcpy(destRowStart, srcRowStart, glyphTex.m_size.x);
            }
        }

        TextChar c = {
            glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
            m_face->glyph->advance.x
        };
        tData.push_back(glyphTex);
        m_characters.insert(std::pair<char, TextChar>(i, c));
    }

    std::string loc = Project::get()->getFontAtlasFolder() + "\\" + std::filesystem::path(m_fontLoc).filename().string();
    loc = loc.substr(0, loc.size() - 4);
    loc += "_" + std::to_string(charSize) + "px.png";
    TextureAtlas::packTexturesToAtlas(tData, charSize * CHARS_X, charSize * CHARS_Y, loc, 1);

    for (int i = 0; i < tData.size(); i++) {
        m_characters[i].m_uvOffset = tData[i].m_uvOffset;
    }
    m_material = new Material;
    m_material->m_serializeTex = false;
    m_material->m_diffuseTexture = Texture::createNewTextureFromPath(loc, m_filterMode, GL_RED, GL_TEXTURE_2D, false);
    m_fontLoaded = true;
    return true;
}

TextFont* TextFont::getFontViaID(unsigned int id) {
    for (int i = 0; i < s_textFonts.size(); i++) {
        if (s_textFonts[i]->m_ID == id) return s_textFonts[i];
    }
    return nullptr;
}

TextFont* TextFont::getFontViaTTFFile(std::string loc) {
    for (int i = 0; i < s_textFonts.size(); i++) {
        if (s_textFonts[i]->m_fontLoc == loc) return s_textFonts[i];
    }
    return nullptr;
}

void TextFont::deserializeFonts(std::string cache) {
    if (cache == "") cache = Project::get()->getFontAtlasFolder();
    for (auto entry : std::filesystem::directory_iterator(cache)) {
        std::string file = entry.path().string();
        std::string ext(File::getExtension(file));
        if (ext == FNT_DEFAULT_EXT) {
            std::ifstream os(file);
            {
                TextFont* f = new TextFont;

                cereal::JSONInputArchive iarchive(os);
                iarchive(*f);
                s_textFonts.push_back(f);
                if (f->m_matID != -1) {
                    f->m_material = Material::getMaterialViaID(f->m_matID);
                    f->m_material->m_diffuseTexture = Texture::createNewTextureFromPath(f->m_material->m_deserializedTexPath.c_str(), f->m_filterMode, GL_RED, GL_TEXTURE_2D, false);
                }
                if (f->m_ID > s_largestIDused) s_largestIDused = f->m_ID;
                f->m_fontLoaded = true;
            }
        }
    }
}

void TextFont::serializeFonts(std::string cache) {
    if (cache == "") cache = Project::get()->getFontAtlasFolder();
    for (int i = 0; i < s_textFonts.size(); i++) {
        if (s_textFonts[i]->m_serialize){
            std::string file = std::string(cache) + std::string("\\font") + std::to_string(i) + std::string(".") + FNT_DEFAULT_EXT;
            std::ofstream os(file, std::ios::binary);
            {
                cereal::JSONOutputArchive oarchive(os);
                oarchive(*s_textFonts[i]);
            }
        }
    }
}
