#include "audiosource.hpp"
#include "camera.hpp"

void AudioSource::setDoppler(float d) {
    AudioManager::get().getResult(FMOD_Channel_Set3DDopplerLevel(m_channel, m_doppler));
}

void AudioSource::setPitch(float p) {
    AudioManager::get().getResult(FMOD_Channel_SetPitch(m_channel, p));
}

void AudioSource::setVolume(float v) {
    AudioManager::get().getResult(FMOD_Channel_SetVolume(m_channel, v));
}

void AudioSource::setMuted(FMOD_BOOL muted) {
    AudioManager::get().getResult(FMOD_Channel_SetMute(m_channel, muted));
}

void AudioSource::setPlaying(FMOD_BOOL playing) {
    AudioManager::get().getResult(FMOD_Channel_SetPaused(m_channel, !playing));
}

float AudioSource::getDoppler() { 
    AudioManager::get().getResult(FMOD_Channel_Get3DDopplerLevel(m_channel, &m_doppler)); 
    return m_doppler; 
}

float AudioSource::getPitch() { 
    AudioManager::get().getResult(FMOD_Channel_GetPitch(m_channel, &m_pitch)); 
    return m_pitch; 
}

float AudioSource::getVolume() {
    AudioManager::get().getResult(FMOD_Channel_GetVolume(m_channel, &m_volume));
    return m_volume;
}

bool AudioSource::isMuted() {
    AudioManager::get().getResult(FMOD_Channel_GetMute(m_channel, &m_muted));
    return m_muted;
}

bool AudioSource::isPlaying() {
    AudioManager::get().getResult(FMOD_Channel_GetPaused(m_channel, &m_isPlaying));
    return m_isPlaying;
}

void AudioSource::start() {

    // getResult() is an automatic error check.
    // FMOD functions return the type "FMOD_RESULT".
    // A callback function to decide what happens 
    // 
    // the audio error callback can be set via 
    // AudioManager::get().setErrCallback(errcallback) 

    AudioManager::get().getResult(FMOD_Channel_Set3DDopplerLevel(m_channel, m_doppler));
    AudioManager::get().getResult(FMOD_Channel_SetPitch(m_channel, m_pitch));
    AudioManager::get().getResult(FMOD_Channel_SetVolume(m_channel, m_volume));
    AudioManager::get().getResult(FMOD_Channel_SetMute(m_channel, m_muted));
    AudioManager::get().getResult(FMOD_Channel_SetPaused(m_channel, !m_isPlaying));
}

void AudioSource::update() {

    m_pos.x = transform.getGlobalPosition().x;
    m_pos.y = transform.getGlobalPosition().y;
    m_pos.z = (Camera::mainCamera->m_isOrtho) ? 0 : transform.getGlobalPosition().z;
    
    // TODO : velocities at some point
    // (pretty sure this is exclusively for doppler shit)
    m_vel.x = 0;
    m_vel.y = 0;
    m_vel.z = 0;

    AudioManager::get().getResult(FMOD_Channel_Set3DAttributes(m_channel, &m_pos, &m_vel));
}