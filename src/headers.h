#pragma once

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../include/glm/gtx/matrix_decompose.hpp"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/cereal/archives/json.hpp"
#include "../include/cereal/types/array.hpp"
#include "../include/cereal/types/string.hpp"
#include "../include/cereal/types/base_class.hpp"
#include "../include/cereal/types/polymorphic.hpp"
#include "../include/cereal/types/vector.hpp"
#include "../include/cereal/types/map.hpp"
#include "windowmanager/fullscreenmode.hpp"
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include <source_location>
#include <windows.h>

#define WND_TITLE "Peachmilk Editor"
#define WND_W 600
#define WND_H 600
#define RECT_W 528
#define RECT_H 297
#define TARGET_FPS 60
#define MIN_FPS 1
#define MAX_FPS 1000

#define modelUniform "model_mat4"
#define perspectiveUniform "persp_mat4"
#define viewUniform "view_mat4"
#define vpUniform "vp_mat4"
#define texAppliedUniform "texture0_applied"
#define wireframeUniform "wireframe"
#define wireframeColUniform "wireframeCol"
#define highlightUniform "highlighted"
#define highlightColUniform "highlightCol"

#define EDITOR_RC "editor_data/"
#define PROJECTS_RC "projects/"
#define MAT_DEFAULT_EXT "pny" // peony
#define TEX_DEFAULT_EXT "mgd" // marigold
#define LEV_DEFAULT_EXT "lvd" // lavender
#define SND_DEFAULT_EXT "sfn" // saffron
#define CHN_DEFAULT_EXT "mnf" // moonflower
#define FNT_DEFAULT_EXT "ddl" // dandelion
#define MAX_AUDIO_CHANNELS 512
#define AUDIO_DISTANCEFACTOR 1
#define AUDIO_DOPPLER 1
#define AUDIO_ROLLOFF 1
#define AUDIO_MIN_FALLOFF_DEFAULT 100
#define AUDIO_MAX_FALLOFF_DEFAULT 3000
#define AUDIO_MAX_FALLOFF_MAX 5000
#define AUDIO_MAX_VOLUME 400

#define _FLAG(ex) std::cout << "flag " << ex << std::endl;
#define FLAG _FLAG("")

typedef void (*init_func)(void);
typedef void (*window_hint_func)(int, int);
typedef void (*critical_err_func)(void);
