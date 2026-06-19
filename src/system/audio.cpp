#include "audio.hpp"

FMOD_RESULT nonblockcallback(FMOD_SOUND *sound, FMOD_RESULT result)
{
    FMOD_SOUND *snd = (FMOD_SOUND *)sound;

    printf("Sound loaded! (%d) %s\n", result, FMOD_ErrorString(result)); 

    return FMOD_OK;
}

void AudioManager::setErrCallback(critical_err_func fn) {
    m_errFnc = fn;
}

void AudioManager::create() {
    if (m_created) return; 

    _FMOD_CreateSystem(&m_pSystem, FMOD_VERSION); 
    _FMOD_InitSystem(m_pSystem, MAX_AUDIO_CHANNELS, FMOD_INIT_NORMAL, NULL);
    getResult(FMOD_System_Set3DSettings(m_pSystem, AUDIO_DOPPLER, AUDIO_DISTANCEFACTOR, AUDIO_ROLLOFF));

    memset(&m_exInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    m_exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    m_exInfo.nonblockcallback = nonblockcallback;

    std::cout << "Created FMOD System" << std::endl;

    createSound("birds.wav", &m_testSound, FMOD_3D, false);

    m_created = true;
}

void AudioManager::getResult(FMOD_RESULT r) {
    if (r != FMOD_OK) {
        if (!suppressErrors) 
            printf("FMOD error! (%d) %s\n", r, FMOD_ErrorString(r));
        m_errFnc();
    }
    return;
}

void AudioManager::createSound(const char* path, FMOD_SOUND ** sound, uint8_t mode, bool isStream) {
    if (isStream)
        getResult(FMOD_System_CreateStream(m_pSystem, path, FMOD_NONBLOCKING, &m_exInfo, sound));
    else
        getResult(FMOD_System_CreateSound(m_pSystem, path, mode, &m_exInfo, sound));
}

void AudioManager::playSound(FMOD_SOUND * sound, FMOD_CHANNEL ** channel) {
    if (channel == nullptr) channel = &m_defaultChannel;
    _FMOD_PlaySound(m_pSystem, sound, FMOD_DEFAULT, false, channel);
}

void AudioManager::playTestSound() {
    playSound(m_testSound);
}

void AudioManager::update(Transform & listener, bool is2D) {

    // there's probably only ever gonna be one listener anyway
    // unless i decide to do something COMPLETELY wacky

    m_pos.x = listener.getGlobalPosition().x;
    m_pos.y = listener.getGlobalPosition().y;
    m_pos.z = (is2D) ? 0 : listener.getGlobalPosition().z;
    
    // cba with velocity rn
    m_vel.x = 0;
    m_vel.y = 0;
    m_vel.z = 0;

    m_forward.x = listener.globalForward().x;
    m_forward.y = listener.globalForward().y;
    m_forward.z = listener.globalForward().z;

    m_up.x = listener.globalUp().x;
    m_up.y = listener.globalUp().y;
    m_up.z = listener.globalUp().z;


    FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &m_pos, &m_vel, &m_forward, &m_up);
    FMOD_System_Update(m_pSystem);
}
