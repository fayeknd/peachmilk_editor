#include "textureatlas.hpp"
#include <algorithm>
#include "../../include/stb_image/stb_rect_pack.h"
#include "../../include/stb_image/stb_image_write.h"

bool TextureAtlas::packTexturesToAtlas(std::vector<TextureData>& tData, unsigned int atlasW, unsigned int atlasH, std::string path, unsigned int channels) {

    stbrp_context context;
    std::vector<stbrp_node> nodes(atlasW);
    std::memset(&context, 0, sizeof(context));
    stbrp_init_target(&context, atlasW, atlasH, nodes.data(), (int)nodes.size());

    int t_size = tData.size();
    std::vector<stbrp_rect> rects(t_size);
    for (int i = 0; i < t_size; ++i) {
        rects[i].id = i;
        rects[i].w  = tData[i].m_size.x + ATLAS_PADDING;
        rects[i].h  = tData[i].m_size.y + ATLAS_PADDING;
        rects[i].was_packed = 0;
    }

    if (stbrp_pack_rects(&context, rects.data(), (int)rects.size()) == 0) {
        std::cout << "ERR : Packing Error! Target Atlas Size: " << atlasW << "x" << atlasH << "\n";
        for (size_t i = 0; i < rects.size(); ++i) {
            std::cerr << "Texture [" << rects[i].id << "]: Size = "
                      << rects[i].w << "x" << rects[i].h
                      << " | Was Packed Status = " << rects[i].was_packed << "\n";
        }
        return false;
    }

    for (int i = 0; i < rects.size(); ++i) {
        int index = rects[i].id;
        tData[index].m_uvOffset.x = rects[i].x;
        tData[index].m_uvOffset.y = rects[i].y;
    }

    return writeToAtlasImagePNG4C(tData, atlasW, atlasH, path);
}

bool TextureAtlas::packTexturesToAtlas(std::vector<TextureData1C>& tData, unsigned int atlasW, unsigned int atlasH, std::string path, unsigned int channels) {

    stbrp_context context;
    // this bullshit
    std::vector<stbrp_node> nodes(atlasW);
    std::memset(&context, 0, sizeof(context));
    stbrp_init_target(&context, atlasW, atlasH, nodes.data(), (int)nodes.size());

    int t_size = tData.size();
    std::vector<stbrp_rect> rects(t_size);
    for (int i = 0; i < t_size; ++i) {
        rects[i].id = i;
        rects[i].w  = tData[i].m_size.x + ATLAS_PADDING;
        rects[i].h  = tData[i].m_size.y + ATLAS_PADDING;
        rects[i].was_packed = 0;
    }

    if (stbrp_pack_rects(&context, rects.data(), (int)rects.size()) == 0) {
        std::cout << "ERR : Packing Error! Target Atlas Size: " << atlasW << "x" << atlasH << "\n";
        for (size_t i = 0; i < rects.size(); ++i) {
            std::cerr << "Texture [" << rects[i].id << "]: Size = "
                      << rects[i].w << "x" << rects[i].h
                      << " | Was Packed Status = " << rects[i].was_packed << "\n";
        }
        return false;
    }

    for (int i = 0; i < rects.size(); ++i) {
        int index = rects[i].id;
        tData[index].m_uvOffset.x = rects[i].x;
        tData[index].m_uvOffset.y = rects[i].y;
    }

    return writeToAtlasImagePNG1C(tData, atlasW, atlasH, path);
}


bool TextureAtlas::writeToAtlasImagePNG1C(std::vector<TextureData1C>& tData, unsigned int atlasW, unsigned int atlasH, std::string path) {

    std::vector<uint8_t> atlasBuf(atlasW * atlasH, 0);

    for (const auto& t : tData) {
        if (t.m_pixels.empty()) continue;

        for (int row = 0; row < t.m_size.y; ++row) {
            int srcOffset = row * t.m_size.x;
            int destOffset = (t.m_uvOffset.y + row) * atlasW + t.m_uvOffset.x;

            std::memcpy(&atlasBuf[destOffset], &t.m_pixels[srcOffset], t.m_size.x);
        }
    }
    stbi_write_png(path.c_str(), atlasW, atlasH, 1, atlasBuf.data(), atlasW * 1);
    return true;
}

bool TextureAtlas::writeToAtlasImagePNG4C(std::vector<TextureData>& tData, unsigned int atlasW, unsigned int atlasH, std::string path) {

    std::vector<uint32_t> atlasBuf(atlasW * atlasH, 0);

    for (const auto& t : tData) {
        if (t.m_pixels == 0) continue;

        for (int row = 0; row < t.m_size.y; ++row) {
            int srcOffset = row * t.m_size.x;
            int destOffset = (t.m_uvOffset.y + row) * atlasW + t.m_uvOffset.x;

            std::memcpy(&atlasBuf[destOffset], &t.m_pixels[srcOffset], t.m_size.x);
        }
    }

    stbi_write_png(path.c_str(), atlasW, atlasH, 4, atlasBuf.data(), atlasW * 4);

    return true;
}
