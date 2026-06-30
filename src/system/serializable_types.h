#pragma once

#include "../render/sprite.hpp"
#include "../audio/audiosource.hpp"
#include "../render/text/text.hpp"

#define SAVE_TYPES {                \
    while (true) {                  \
        _SAVE_TYPE(Sprite)          \
        _SAVE_TYPE(AudioSource)     \
        _SAVE_TYPE(TextRenderer)    \
        _SAVE_TYPE(ScriptableEntity)\
        break;                      \
    }                               \
}

#define LOAD_TYPES {                \
    while (true) {                  \
        _LOAD_TYPE(Sprite)          \
        _LOAD_TYPE(AudioSource)     \
        _LOAD_TYPE(TextRenderer)    \
        _LOAD_TYPE(ScriptableEntity)\
        break;                      \
    }                               \
}

#define DELETE_ENTITY(E) {                \
    while (true) {                        \
        _DELETE_TYPE(Sprite, E)           \
        _DELETE_TYPE(AudioSource, E)      \
        _DELETE_TYPE(TextRenderer, E)     \
        _DELETE_TYPE(ScriptableEntity, E) \
        break;                            \
    }                                     \
}                                         \

#define __TYPE 0
