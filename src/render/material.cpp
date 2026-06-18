#include "material.hpp"
#include "../system/file.hpp"
#include <fstream>

Material* Material::s_defaultMaterial = new Material;
std::vector<Material*> Material::s_allMaterials {};

Material* Material::getMaterialViaID(unsigned int id) {
    for (int i = 0; i < s_allMaterials.size(); i++) {
        if (s_allMaterials[i]->getID() == id) return s_allMaterials[i];
    }
    return nullptr; 
}

void Material::deseralizeMaterials(std::string cache) {
    if (cache == "") cache = Project::get()->getMaterialFolder().c_str();
    for (auto entry : std::filesystem::directory_iterator(cache)) {
        std::string file = entry.path().string();
        std::string ext(File::getExtension(file));
        if (ext == MAT_DEFAULT_EXT) {
            std::ifstream os(file, std::ios::binary);
            {
                cereal::JSONInputArchive iarchive(os); // Create an output archive

                Material* m = new Material;
                iarchive(*m);
                //deserializedtexpath
                std::string dstp = m->getDeserializedTexPath();
                //std::cout << dstp << std::endl;
                if (File::fileExists(dstp))
                    m->m_diffuseTexture = Texture::createNewTextureFromPath(dstp.c_str());
                m->m_serializedPath = file;
                if (m->getID() > Material::s_largestIDused) Material::s_largestIDused = m->getID();
            } 
        }  
    }
}

void Material::serializeMaterials(std::string cache) {
    if (cache == "") cache = Project::get()->getMaterialFolder();
    for (int i = 0; i < Material::s_allMaterials.size(); i++) {
        if (s_allMaterials[i]->m_doSerialize){
            std::string file = s_allMaterials[i]->m_serializedPath;
            if (file == "")
                file = std::string(cache) + std::string("\\material") + std::to_string(i) + std::string(".") + MAT_DEFAULT_EXT;
            std::ofstream os(file, std::ios::binary);
            {
                cereal::JSONOutputArchive oarchive(os); // Create an output archive
                oarchive(*Material::s_allMaterials[i]);
            }
        }
    }
}