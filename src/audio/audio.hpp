#pragma once
#include "../../include/fmod/fmod.h"
#include "../../include/fmod/fmod_errors.h"
#include <future>
#include <thread>
#include "../render/transform.hpp"

#include "../headers.h"

// This is very barebones

#define _FMOD_CreateSystem(SYS, VER) getResult(FMOD_System_Create(SYS, VER))
#define _FMOD_InitSystem(SYS, CHANNELS, FLAGS, EXDATA) getResult(FMOD_System_Init(SYS, CHANNELS, FLAGS, EXDATA))
#define _FMOD_CreateSound(SYS, SOUND, MODE, INFO, PTR) getResult(FMOD_System_CreateSound(SYS, SOUND, MODE, INFO, PTR))
#define _FMOD_PlaySound(SYS, SOUND, CGROUP, PAUSED, CHANNEL) getResult(FMOD_System_PlaySound(SYS, SOUND, CGROUP, PAUSED, CHANNEL))

class AudioManager {
private:

    // use the c-style syntax because the c++ syntax does not work with mingw
    // (im starting to fucking hate mingw)

    bool m_created = false;
    critical_err_func m_errFnc = NULL;
    void _defaultErrFnc();
    FMOD_SYSTEM* m_pSystem = nullptr;
    FMOD_CHANNEL* m_defaultChannel = nullptr;
    FMOD_CREATESOUNDEXINFO m_exInfo;

    FMOD_SOUND * m_testSound = nullptr;
    FMOD_VECTOR m_pos;
    FMOD_VECTOR m_vel;
    FMOD_VECTOR m_forward;
    FMOD_VECTOR m_up;
    FMOD_VECTOR m_listenerLF{0};
    int m_channelsPlaying = 0;
    int m_realChannels = 0;
public:

    bool suppressErrors = false;
    static AudioManager& get() {
        static AudioManager _;
        return _;
    }

    int getChannelsPlaying() { return m_channelsPlaying; }

    FMOD_CHANNEL * getDefaultChannel() { return m_defaultChannel; }
    FMOD_SYSTEM * getSystem() { return m_pSystem; }

    void createChannel(FMOD_CHANNEL ** channel);
    void playTestSound();
    void setErrCallback(critical_err_func fn);
    void create();
    void createSound(const char* path, FMOD_SOUND ** sound, uint8_t mode = FMOD_3D, bool stream = false);
    void playSound(FMOD_SOUND * sound, FMOD_CHANNEL ** channel);
    void getResult(FMOD_RESULT r);
    void update(Transform & listener, bool is2D);
    static float float2db(float f);

    friend class AudioClip;
};
