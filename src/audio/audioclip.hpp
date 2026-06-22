#pragma once
#include "audio.hpp"

class AudioClip {
protected:

    FMOD_SOUND * m_sound; 
    std::string m_path = "";
    std::string m_serializedPath = "";
    std::string m_name;
    unsigned int m_ID = -1;
    static inline unsigned int s_availableID = 0;
    static inline unsigned int s_largestIDused = 0;
    bool m_created = false; 
    bool m_isStream = false;
    uint16_t m_mode = 0;

public:

    static inline std::vector<AudioClip*> s_allClips {};
    bool m_serialize = true;
    AudioClip(const char* path, uint16_t mode = FMOD_3D, bool isStream = false); 
    AudioClip() {};
    ~AudioClip();
    std::string getName() { return m_name; }

    FMOD_SOUND * getFMODSound() { return m_sound; }
    template <class Archive>
    void serialize(Archive & archive) {
        archive(m_mode, m_isStream, m_path, m_ID);
    }
    unsigned int getID() { return m_ID; }
    std::string getPath() { return m_path; }
    bool createSound(const char* path, uint16_t mode = FMOD_3D, bool isStream = false, bool assignID = true);
    static AudioClip* getClipViaID(unsigned int ID);    
    // Note for tomorrow : DO THIS NOW

    void assignID();
    static void deseralizeSounds(std::string cache = "");
    static void serializeSounds(std::string cache = "");

    void destroy() {
        if (m_serializedPath != "")
            std::remove(m_serializedPath.c_str());
        delete this;
    }

    friend class AudioSource;

};