#pragma once
#include "../system/audio.hpp"

class AudioClip {
protected:

    FMOD_SOUND * m_sound; 
    std::string m_path;

    bool m_created = false; 

public:

    template <class Archive>
    void serialize(Archive & archive) {
        archive(m_created, m_path);
    }

    std::string getPath() { return m_path; }
    void createSound(const char* path, uint8_t mode, bool isStream);
    
    // Note for tomorrow : DO THIS NOW

    static void deseralizeMaterials(std::string cache = "");
    static void serializeMaterials(std::string cache = "");
};