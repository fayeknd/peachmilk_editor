#pragma once
#include "../headers.h"

#include "texturedata.hpp"

#define ATLAS_PADDING 2

class TextureAtlas {
private:

    // differemt file implementations here or smth dfiifdkfj
    static bool writeToAtlasImagePNG1C(std::vector<TextureData1C>& tData, unsigned int atlasW, unsigned int atlasH, std::string path = "");
    static bool writeToAtlasImagePNG4C(std::vector<TextureData>& tData, unsigned int atlasW, unsigned int atlasH, std::string path = "");

public:
    static bool packTexturesToAtlas(std::vector<TextureData>& tData, unsigned int atlasW, unsigned int atlasH, std::string path = "", unsigned int channels = 4);
    static bool packTexturesToAtlas(std::vector<TextureData1C>& tData, unsigned int atlasW, unsigned int atlasH, std::string path = "", unsigned int channels = 1);
};
