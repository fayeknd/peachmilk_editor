#pragma once
#include "../headers.h"

struct TextureData {
    glm::ivec2 m_size {0,0};
    glm::ivec2 m_uvOffset {0,0};
    unsigned char* m_pixels = 0;
};

struct TextureData1C {
    glm::ivec2 m_size {0,0};
    glm::ivec2 m_uvOffset {0,0};
    std::vector<uint8_t> m_pixels;
};
