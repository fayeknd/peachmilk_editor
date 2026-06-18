#pragma once
#define BLEND_NONE 0
#define BLEND_ADD 1
#define BLEND_SUBTRACT 2
#define BLEND_MIN 3
#define BLEND_MAX 4
#define BLEND_REVERSE_SUBTRACT 5
#include "../headers.h"
#include "iostream"

class BlendMode {
public:

    static inline unsigned int s_currentMode = -1;
    unsigned int m_mode = BLEND_NONE;

    std::string asString() {
        switch (m_mode) {
            case BLEND_NONE:
            return "None";
            break;
            case BLEND_ADD:
            return "Add";
            break;
            case BLEND_SUBTRACT:
            return "Subtract";
            break;
            case BLEND_MIN:
            return "Min";
            break;
            default:
            case BLEND_MAX:
            return "Max";
            break;
            case BLEND_REVERSE_SUBTRACT:
            return "Reverse Subtract";
            break;
            return "Unknown";
        }
    };

    static void useDefaultBlending() {
        if (s_currentMode == BLEND_NONE) return;
        s_currentMode = BLEND_NONE;
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    }
    
    void use() {
        if (s_currentMode == m_mode) return;
        s_currentMode = m_mode;
        switch (m_mode) {
            case BLEND_NONE:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
            break;
            case BLEND_ADD:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
            case BLEND_SUBTRACT:
            glBlendEquation(GL_FUNC_SUBTRACT);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
            case BLEND_REVERSE_SUBTRACT:
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
            case BLEND_MIN:
            glBlendEquation(GL_MIN);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
            break;
            case BLEND_MAX:
            glBlendEquation(GL_MAX);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
            break;
        }
    }

};