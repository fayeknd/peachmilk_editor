#include "texture.hpp"
#include "../stb.h"
#include <iostream>
#include "../system/file.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include "../editor/project.hpp"

std::vector<Texture*> Texture::s_allTextures {};

/* // mgd or marigold (or "mega giga dih") will store every texture
bool Texture::saveAsMGD(const char* pathOverride) {
    std::ofstream os;
    os.open(pathOverride,  std::ofstream::out | std::ofstream::trunc);

    std::string str( s_allTextures[0]->m_textureData, s_allTextures[0]->m_textureData + sizeof(s_allTextures[0]->m_textureData) / sizeof(s_allTextures[0]->m_textureData[0]) );

    std::cout << s_allTextures[0]->m_textureData[0] << std::endl;

    os << str;
    os.close();
    return true;
}
 */
unsigned char* Texture::getDataFromFileSTBI(std::string file, int *x, int *y, int* c) {
    return stbi_load(file.c_str(), x, y, c, 0);
}

unsigned char* Texture::getDataFromFileMGD(std::string file, int *x, int *y, unsigned int* filtering, bool flipVertical, bool flipHorizontal, int rotate) {
    std::ifstream is(file);
    std::string line;

    int w, h;
    bool inDataChunk = false;
    int it = 0;
    while (std::getline(is, line))
    {
        it ++;
        std::string substr = line.substr(0, 1);
        if (substr == "f") {
            *filtering = std::stoi(line.substr(2, line.size()));
            if (*filtering == 0) *filtering = DEFAULT_FILTERING;
        }
        if (substr == "s") {
            line = line.substr(1, line.size());
            int separator = line.find("/");
            w = std::stoi(line.substr(1, separator - 1));
            h = std::stoi(line.substr(separator + 1, line.size()));
        }
        if (substr == "\n") break;
        // clearly the mgd file is incorrect
        if (it == 10) return nullptr;
    }

    std::vector<Pixel> data {};
    Pixel pixel;

    std::string element = "";
    int iter = 0;
    for (char& c : line) { // <- this line
        if (c != '/')
            element += c;
        else {
            int element_i = std::stoi(element);
            pixel.data[iter] = static_cast<unsigned char>(element_i);
            //std::cout << element_i << std::endl;

            // if this is the 4th component of the pixel (alpha)
            if (iter == 3) {
                //std::cout << static_cast<int>(pixel.data[0]) << ", " << static_cast<int>(pixel.data[1]) << ", " << static_cast<int>(pixel.data[2]) << ", " <<static_cast<int>(pixel.data[3]) << std::endl;
                data.push_back(pixel);
                iter = 0;
            }
            else
                iter++;
            element.clear();
        }
    }

    // see the thing about shit like this is after you do it you completely forget how it works almost instantly
    if (flipVertical) {
        std::vector<Pixel> row {};
        for (int i = 0; i < h; i++) {
            int offset = i * w;
            for (int j = 0; j < w; j++) {
                row.insert(row.begin(), data[offset + j]);
            }
            for (int j = 0; j < w; j++) {
                data[offset + j] = row[j];
            }
            row.clear();
        }
    }
    if (flipHorizontal) {
        std::vector<Pixel> column {};
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                column.insert(column.begin(), data[(j * h) + i]);
            }
            for (int j = 0; j < h; j++) {
                data[(j * h) + i] = column[j];
            }
            column.clear();
        }
    }

    for (int r = 0; r < rotate; r++) {
        std::vector<std::vector<Pixel>> rows {};
        std::vector<Pixel> row {};

        for (int i = 0; i < h; i++) {
            int offset = i * w;
            for (int j = 0; j < w; j++) {
                row.push_back(data[offset + j]);
            }
            rows.push_back(row);
            row.clear();
        }
        // h and w is swapped
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                data[(j * h) + i] = rows[i][j];
            }
        }

    }

    unsigned char* _ = new unsigned char[data.size() * 4];
    for(int i = 0; i < data.size(); i++) {
        int index = ((i + 1) * 4) - 4;
        _[index + 0] = data[i].data[0];
        _[index + 1] = data[i].data[1];
        _[index + 2] = data[i].data[2];
        _[index + 3] = data[i].data[3];
    }

    *x = w;
    *y = h;

    return _;

}

void Texture::shouldFlip(bool flag) {
    stbi_set_flip_vertically_on_load(flag);
    m_shouldFlip = flag;
}

Texture* Texture::getLoadedTexture(std::string path) {
    // if the given path already exists in one of the textures, set the texture pointer to that texture.
    std::filesystem::path p(path);
    std::filesystem::path p2;
    for (int i = 0; i < s_allTextures.size(); i++) {
        p2 = s_allTextures[i]->getFilePath();
        if (p == p2) {
            return s_allTextures[i];
        }
    }
    return nullptr;
}

Texture* Texture::getWithID(unsigned int id) {
    for (int i = 0; i < s_allTextures.size(); i++) {
        if (s_allTextures[i]->getID() == id) {
            return s_allTextures[i];
        }
    }
    return nullptr;
}

Texture* Texture::createNewTextureFromPath(std::string path, unsigned int filterMode, unsigned int format, unsigned int textureType, bool serialize) {
    Texture* t = getLoadedTexture(path);
    if (t != nullptr) {
        std::cout << "Texture is already loaded!" << std::endl;
        return t;
    }
    t = new Texture;
    if (!t->createTextureFromPath(path, filterMode, format, textureType, serialize)) {
        delete t;
        return nullptr;
    }
    return t;
}

void Texture::writeDataToMGD(Texture* t, std::string  filePath) {
    if (!t->m_data.m_pixels) return;
    int y = 0;
    int w = t->m_data.m_size.x;
    int h = t->m_data.m_size.y;

    std::string r, g, b, a;

    std::string separator = {"/"};
    int tries = 0;
    std::string s;
    while (File::fileExists(filePath) || filePath == "") {
        s = std::string(Project::get()->getTextureFolder()) + std::string("/texture") + std::to_string(tries) + std::string(".") + std::string(TEX_DEFAULT_EXT);
        filePath = s.c_str();
        tries++;
    }

    std::ofstream os;
    os.open(filePath);

    os << "id " << t->getID() << std::endl;
    os << "f " << t->getFilterMode() << std::endl;
    os << "s " << t->m_data.m_size.x << separator << t->m_data.m_size.y << std::endl << std::endl;

    for (int x = 0; x < h; x++) {
        for (int y = 0; y < w; y++) {

            unsigned char* pixelOffset = t->m_data.m_pixels + (y + (w * x)) * t->m_channels;
            r = std::to_string(static_cast<int>(pixelOffset[0]));
            g = std::to_string(static_cast<int>(pixelOffset[1]));
            b = std::to_string(static_cast<int>(pixelOffset[2]));
            a = t->m_channels >= 4 ? std::to_string(static_cast<int>(pixelOffset[3])) : std::to_string(0xff);

            os << r + separator + g + separator + b + separator + a + separator;
            //printf("%d, %d, %d, %d\n", r, static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
        }
    }
    t->m_fullPath = File::getFullPath(filePath);
    os.close();
}

bool Texture::createTextureFromPath(std::string path, unsigned int filterMode, unsigned int format, unsigned int textureType, bool serialize) {

    m_serialize = serialize;
    std::string filePath = path;

    if (!File::fileExists(path)) {
        filePath = (std::string(File::getWorkingDirectory()) + std::string(path));
        if (!File::fileExists(filePath)) {
            std::cout << "Texture '" << filePath << "' was not found." << std::endl;
            return false;
        }
    }
    bool copyFlag = true;
    if (m_fullPath == filePath) copyFlag = false;
    else m_fullPath = File::getFullPath(filePath);

    std::string ext = File::getExtension(filePath);
    if (ext == "webp") {
        std::cout << "ERROR : WebP-formatted images are unsupported and will not load properly!" << std::endl;
        return false;
    }
    int w, h;
    if (ext != TEX_DEFAULT_EXT) {
        // load image with stbi
        m_data.m_pixels = getDataFromFileSTBI(filePath.c_str(), &w, &h, &m_channels);

        switch (m_channels) {
            case 1:
            m_format = GL_RED;
            break;
            case 2:
            m_format = GL_RG;
            break;
            case 3:
            m_format = GL_RGB;
            break;
            case 4:
            m_format = GL_RGBA;
            break;
            default:
            m_format = GL_RGB;
            break;
        }
        m_filterMode = filterMode;
        m_data.m_size = glm::vec2(w, h);
        if (serialize && copyFlag) {
            int tries = 0;
            std::string s = filePath;
            std::filesystem::path p(s);
            if (p.parent_path() != Project::get()->getTextureFolder()) {
                while (File::fileExists(s)) {
                    s = std::string(Project::get()->getTextureFolder()) + std::string("/texture") + std::to_string(tries) + std::string(".") + ext;
                    tries++;
                }
                std::filesystem::copy_file(filePath, s);
                m_fullPath = File::getFullPath(s.c_str());
            }
        }
        // mgd files were a terrible idea

        //writeDataToMGD(this);
    }
    else {
        m_format = GL_RGBA;
        m_channels = 4;
        m_data.m_pixels = getDataFromFileMGD(filePath.c_str(), &w, &h, &m_filterMode, false, !m_shouldFlip);
        m_data.m_size = glm::vec2(w, h);
    }

    m_textureType = textureType;

    if (!m_data.m_pixels) {
        assert("ERROR : Texture Data not loaded!");
        return false;
    }
    else
        genTexture();
    return true;
}

Texture::~Texture() {
    stbi_image_free(m_data.m_pixels);
    for (int i = 0; i < s_allTextures.size(); i++) {
        if (s_allTextures[i] == this) {
            s_allTextures.erase(s_allTextures.begin() + i);
            return;
        }
    }
}

void Texture::setFilterMode(unsigned int filterMode) {
    m_filterMode = filterMode;
    glBindTexture(m_textureType, m_ID);
    glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, m_filterMode);
    glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, m_filterMode);
}

void Texture::setFormat(unsigned int format) {
    // in fact wtf am i thinking when would you EVER need this
    // like genuinely why would you need to do this
    // im keeping this here just so you can see how big an idiot i am

    m_format = format;
    std::cout << "Fix this" << std::endl;
    //genTexture();
}

void Texture::genTexture() {

    if (m_initialised)
        glDeleteTextures(1, &m_ID);

    // im not gonna lie and pretend i know exactly how
    // opengl textures are packed - but this fixes them
    // if they're single-channel images (such as text glyphs)
    if (m_channels == 1) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &m_ID);
    glBindTexture(m_textureType, m_ID);

    glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, m_filterMode);
    glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, m_filterMode);

    // to be honest this thing isn't really gonna need mipmapping that much

    glTexImage2D(m_textureType, 0, m_format, m_data.m_size.x, m_data.m_size.y, 0, m_format, GL_UNSIGNED_BYTE, m_data.m_pixels);
    //glGenerateMipmap(m_textureType);

    if (m_channels == 1) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    m_initialised = true;
    s_allTextures.push_back(this);

}
