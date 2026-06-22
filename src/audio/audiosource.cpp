#include "audiosource.hpp"
#include "../render/camera.hpp"
#include "channelgroup.hpp"

void AudioSource::setFalloff(float minf, float maxf, float minf_mult, float maxf_mult) {
    if (m_isPlaying) AudioManager::get().getResult(FMOD_Channel_Set3DMinMaxDistance(m_channel, std::min(minf, minf_mult) * AUDIO_DISTANCEFACTOR, std::max(maxf, maxf_mult) * AUDIO_DISTANCEFACTOR));
    else m_isDirty = true;
    m_minFalloff = minf;
    m_maxFalloff = maxf;
}

void AudioSource::setDoppler(float d, float d_mult) {
    if (m_isPlaying) AudioManager::get().getResult(FMOD_Channel_Set3DDopplerLevel(m_channel, d * d_mult));
    else m_isDirty = true;
    m_doppler = d;
}

void AudioSource::setPitch(float p, float p_mult) {
    if (m_isPlaying) AudioManager::get().getResult(FMOD_Channel_SetPitch(m_channel, p * p_mult));
    else m_isDirty = true;
    m_pitch = p;
}

void AudioSource::setVolume(float v, float v_mult) {
    if (m_isPlaying) AudioManager::get().getResult(FMOD_Channel_SetVolume(m_channel, glm::clamp((v * v_mult), 0.0f, static_cast<float>(AUDIO_MAX_VOLUME))));
    else m_isDirty = true;
    m_volume = v;
}

void AudioSource::setMuted(FMOD_BOOL muted) {
    if (m_isPlaying) AudioManager::get().getResult(FMOD_Channel_SetMute(m_channel, muted));
    else m_isDirty = true;
    m_muted = muted;
}

void AudioSource::setDoesLoop(FMOD_BOOL loop) {
    if (m_isPlaying) {
        if (loop) {
            AudioManager::get().getResult(FMOD_Channel_SetMode(m_channel, FMOD_LOOP_NORMAL));
            AudioManager::get().getResult(FMOD_Channel_SetLoopCount(m_channel, -1));
        }
        else {
            AudioManager::get().getResult(FMOD_Channel_SetMode(m_channel, FMOD_LOOP_OFF));
            AudioManager::get().getResult(FMOD_Channel_SetLoopCount(m_channel, 0));
        }
    }
    m_loop = loop;
}

void AudioSource::set3DLevel(float l, float l_mult) {
    if (m_isPlaying) AudioManager::get().getResult(FMOD_Channel_Set3DLevel(m_channel, l * l_mult));
    else m_isDirty = true;
    m_3dLevel = l;
}

void AudioSource::playSound(AudioClip * clip) {
    if (clip == nullptr) clip = m_clips[0];
    if (clip == nullptr) return;

    // getResult() is an automatic error check.
    // FMOD functions return the type "FMOD_RESULT".
    // A callback function to decide what happens 
    // 
    // the audio error callback can be set via 
    // AudioManager::get().setErrCallback(errcallback) 

    AudioManager::get().playSound(clip->m_sound, &m_channel);   
    m_isPlaying = true;
    m_isDirty = true;
}       

void AudioSource::playSound(unsigned int index) {
    if (!m_clips[index]) return;
    playSound(m_clips[index]);
}

void AudioSource::create(std::string name) {

    ScriptableEntity::create(name);
}

void AudioSource::update() {

    if (m_channel) {
        if (m_3dLevel != 0) {
            m_pos.x = transform.getGlobalPosition().x;
            m_pos.y = transform.getGlobalPosition().y;
            m_pos.z = (Camera::mainCamera->m_isOrtho) ? 0 : transform.getGlobalPosition().z;
            
            // TODO : velocities at some point
            // (pretty sure this is exclusively for doppler shit)
            m_vel.x = m_pos.x - m_posLF.x;
            m_vel.y = m_pos.y - m_posLF.y;
            m_vel.z = m_pos.z - m_posLF.z;    
        }
        if (FMOD_Channel_IsPlaying(m_channel, &m_isPlaying) == FMOD_ERR_INVALID_HANDLE) m_isPlaying = false;
        if (m_isPlaying) {
            AudioManager::get().getResult(FMOD_Channel_Set3DAttributes(m_channel, &m_pos, &m_vel));
            if (m_isDirty) {
                setVolume(m_volume, m_volume_mult);
                setDoppler(m_doppler, m_doppler_mult);
                setPitch(m_pitch, m_pitch_mult);
                setFalloff(m_minFalloff, m_minFalloff_mult, m_maxFalloff, m_minFalloff_mult);
                set3DLevel(m_3dLevel, m_3dLevel_mult);
                setDoesLoop(m_loop); 
                m_isDirty = false;
            }
        }
        m_posLF = m_pos;
    }
}   

void AudioSource::_deserializeFnc() {
    ScriptableEntity::_deserializeFnc();
    for (int i = 0; i < MAX_CLIPS; i++) {
        m_clips[i] = AudioClip::getClipViaID(m_clipIDs[i]);
    }
    if (m_channelGroupName != "") setChannelGroup(m_channelGroupName);
}

AudioSource::~AudioSource() {
    ChannelGroup* cg = ChannelGroup::getChannelGroup(m_channelGroupName);
    for (int i = 0; i < cg->m_channels.size(); i++) {
        if (cg->m_channels[i] == this) {
            cg->m_channels.erase(cg->m_channels.begin() + i);
            break;
        }
    }
}

void AudioSource::setChannelGroup(std::string name) {
    ChannelGroup* original_cg = ChannelGroup::getChannelGroup(m_channelGroupName);
    if (original_cg) {
        for (int i = 0; i < original_cg->m_channels.size(); i++) {
            if (original_cg->m_channels[i] == this) {
                original_cg->m_channels.erase(original_cg->m_channels.begin() + i);
                break;
            }
        }
    }
    ChannelGroup* cg = ChannelGroup::getChannelGroup(name);
    if (cg) {
        cg->m_channels.push_back(this);
        m_channelGroupName = cg->getName();
        cg->updateChannelMultipliers();
    }
}

CEREAL_REGISTER_TYPE(AudioSource)