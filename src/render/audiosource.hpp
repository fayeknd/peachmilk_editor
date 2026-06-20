#pragma once
#include "../headers.h"
#include "entity.hpp"
#include "../system/audio.hpp"

class AudioSource : public ScriptableEntity {
private:
    FMOD_CHANNEL * m_channel = nullptr;
    float m_doppler = 1;
    float m_pitch = 1;
    float m_volume = 1;
    FMOD_BOOL m_muted = false;
    FMOD_BOOL m_isPlaying = false;

    FMOD_VECTOR m_pos;
    FMOD_VECTOR m_vel;

public:

    template <class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ScriptableEntity>(this), m_doppler, m_pitch, m_volume, m_muted, m_isPlaying);
    } 

    void setPlaying(FMOD_BOOL playing);
    void setMuted(FMOD_BOOL muted);
    void setDoppler(float d);
    void setPitch(float p);
    void setVolume(float v);

    bool isPlaying();
    bool isMuted();
    float getDoppler();
    float getPitch();
    float getVolume();


    void start() override;
    void update() override;
    bool draw() override {return true;};

};