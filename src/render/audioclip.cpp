#include "audioclip.hpp"

void AudioClip::createSound(const char* path, uint8_t mode, bool isStream) {
    if (!m_created) return;
    AudioManager & am = AudioManager::get();
    if (isStream)
        am.getResult(FMOD_System_CreateStream(am.getSystem(), path, FMOD_NONBLOCKING, &am.m_exInfo, &m_sound));
    else
        am.getResult(FMOD_System_CreateSound(am.getSystem(), path, mode, &am.m_exInfo, &m_sound));
    m_created = true;
}