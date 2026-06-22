#pragma once
#include "../headers.h"
#include "../render/entity.hpp"
#include "audio.hpp"
#include "audioclip.hpp"

#define MAX_CLIPS 16
class AudioSource : public ScriptableEntity {
private:
    FMOD_CHANNEL * m_channel = 0;
    float m_doppler = 1;
    float m_pitch = 1;
    float m_volume = 1;
    float m_minFalloff = AUDIO_MIN_FALLOFF_DEFAULT;
    float m_maxFalloff = AUDIO_MAX_FALLOFF_DEFAULT;
    float m_3dLevel = 1;

    float m_doppler_mult = 1;
    float m_pitch_mult = 1;
    float m_volume_mult = 1;
    float m_minFalloff_mult = AUDIO_MIN_FALLOFF_DEFAULT;
    float m_maxFalloff_mult = AUDIO_MAX_FALLOFF_DEFAULT;
    float m_3dLevel_mult = 1;

    FMOD_BOOL m_isPlaying = false;
    FMOD_BOOL m_loop = false;
    FMOD_BOOL m_muted = false;

    FMOD_VECTOR m_pos;
    FMOD_VECTOR m_vel;
    FMOD_VECTOR m_posLF{0};
    unsigned int m_clipIDs[MAX_CLIPS] {};

    std::string m_channelGroupName = ""; 
    bool m_isDirty = true;
public:

    AudioClip* m_clips[MAX_CLIPS] {nullptr};
    ~AudioSource();

    template <class Archive>
    void serialize(Archive & archive) {
        for (int i = 0; i < MAX_CLIPS; i++) {
            m_clipIDs[i] = (m_clips[i] != nullptr) ? m_clips[i]->getID() : -1;
        }
        archive(cereal::base_class<ScriptableEntity>(this), m_doppler, m_pitch, m_volume, m_loop, m_muted, m_minFalloff, m_maxFalloff, m_3dLevel, m_clipIDs, m_channelGroupName);
    } 
    void playSound(AudioClip * clip = nullptr);
    void playSound(unsigned int index = 0);
    void setFalloff(float minf, float maxf, float minf_mulf = 1, float maxf_mult = 1);
    void setMuted(FMOD_BOOL muted);
    void setDoppler(float d, float d_mult = 1);
    void setPitch(float p, float p_mult = 1);
    void setVolume(float v, float v_mult = 1);
    void set3DLevel(float l, float l_mult = 1);
    void setDoesLoop(FMOD_BOOL loop);

    float getMinFalloff() { return m_minFalloff; }
    float getMaxFalloff() { return m_maxFalloff; }
    bool isPlaying() { return m_isPlaying; }
    bool doesLoop() { return m_loop; }
    bool getMuted() { return m_muted; }
    float getDoppler() { return m_doppler; }
    float getPitch() { return m_pitch; }
    float getVolume() { return m_volume; }
    float get3DLevel() { return m_3dLevel; }

    void _deserializeFnc() override;
    std::string getChannelGroupName() { return m_channelGroupName; }
    void setChannelGroup(std::string name);
    void create(std::string name = "Unnamed ") override;
    void update() override;
    bool draw() override {return true;};

    FMOD_CHANNEL * getChannel() { return m_channel; }
    friend class ChannelGroup;
};