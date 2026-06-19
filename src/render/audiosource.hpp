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
    bool draw() override;

};