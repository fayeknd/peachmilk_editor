#pragma once
#include "audio.hpp"
#include "../headers.h"
#include "audiosource.hpp"

class ChannelGroup {
protected:
    std::string m_name;
    unsigned int m_ID = -1;
    static inline unsigned int s_availableID = 0; 
    bool m_created = false;
    float m_doppler = 1;
    float m_pitch = 1;
    float m_volume = 1;
    float m_minFalloff = AUDIO_MIN_FALLOFF_DEFAULT;
    float m_maxFalloff = AUDIO_MAX_FALLOFF_DEFAULT;
    float m_3dLevel = 1;

    std::string m_serializedPath = "";
public:

    std::vector<AudioSource*> m_channels {};
    static inline std::vector<ChannelGroup*> s_channelGroups {};

    template<class Archive>
    void serialize(Archive & archive) {
        archive(m_name, m_ID, m_volume, m_pitch, m_doppler, m_minFalloff, m_maxFalloff, m_3dLevel);
    }

    ChannelGroup(std::string name);
    ChannelGroup();
    ~ChannelGroup();

    bool m_serialize = true;

    void setFalloff(float minf, float maxf);
    void setMuted(FMOD_BOOL muted);
    void setDoppler(float d);
    void setPitch(float p);
    void setVolume(float v);
    void set3DLevel(float l);

    float getMinFalloff() { return m_minFalloff; }
    float getMaxFalloff() { return m_maxFalloff; }
    float getDoppler() { return m_doppler; }
    float getPitch() { return m_pitch; }
    float getVolume() { return m_volume; }
    float get3DLevel() { return m_3dLevel; }

    bool create(std::string name, bool skipNameCheck = false);
    static ChannelGroup * getChannelGroup(unsigned int id);
    static ChannelGroup * getChannelGroup(std::string name);
    std::string getName() { return m_name; }
    void setName(std::string name);

    static void deseralizeChannelGroups(std::string cache = "");
    static void serializeChannelGroups(std::string cache = "");

    void updateChannelMultipliers();

};