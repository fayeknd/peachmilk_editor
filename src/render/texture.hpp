#pragma once
#include "../headers.h"
#include <filesystem>

#define DEFAULT_FILTERING 0x2600 // GL_NEAREST
#define DEFAULT_FORMAT 0x1908 // GL_RGBA

// the Pixel class exists just to make reordering image data slightly more bearable
struct Pixel {
    unsigned char data[4];
};

class Texture {
protected:
    
    bool m_initialised = false;

    glm::vec2 m_size = glm::vec2(0);
    int m_channels;
    unsigned int m_ID;
    unsigned int m_format;
    unsigned int m_filterMode;
    unsigned int m_textureType;
    bool m_serialize = true;
    static inline bool m_shouldFlip = false;

    unsigned char* m_textureData = 0;
    std::filesystem::path m_fullPath; 

public:

    static std::vector<Texture*> s_allTextures;
    static void writeDataToMGD(Texture* t, std::string filePath = ""); 
    static Texture* getLoadedTexture(std::string  path);
    bool createTextureFromPath(std::string  path, unsigned int filterMode = DEFAULT_FILTERING, unsigned int format = 0, unsigned int textureType = GL_TEXTURE_2D, bool serialize = true);
    static Texture* createNewTextureFromPath(std::string  path, unsigned int filterMode = 0, unsigned int format = 0, unsigned int textureType = GL_TEXTURE_2D, bool serialize = true);
    static Texture* createNewTextureWithData(unsigned char* data, unsigned int filterMode = 0, unsigned int format = 0, unsigned int textureType = GL_TEXTURE_2D, bool serialize = true);
    static Texture* getWithID(unsigned int id);

    glm::vec2 dimensions() { return m_size; };

    unsigned int getFilterMode() { return m_filterMode; }
    void setFilterMode(unsigned int filterMode);
    void setFormat(unsigned int format);
    void genTexture();

    std::string getFilePath() { return m_fullPath.string(); };

    unsigned int getID() { return m_ID; }
    unsigned int getType() { return m_textureType; }
    bool isInitialised() { return m_initialised; }

    static void shouldFlip(bool flag);
    
    static unsigned char* getDataFromFileSTBI(std::string  file, int* x, int* y, int* c);
    static unsigned char* getDataFromFileMGD(std::string  file, int* x, int* y, unsigned int* filtering, bool flipVertical = false, bool flipHorizontal = false, int rotate = 0);

    ~Texture();

};