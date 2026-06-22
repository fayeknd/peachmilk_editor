cmake_minimum_required(VERSION 3.10.0)
project(engine VERSION 0.1.0 LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS    "-static ")
#SET(CMAKE_CXX_FLAGS    "-static -windows")

add_executable(engine src/main.cpp 
                include/src/glad.c 
                include/imgui/imgui.cpp
                include/imgui/imgui_demo.cpp
                include/imgui/imgui_draw.cpp
                include/imgui/imgui_impl_glfw.cpp
                include/imgui/imgui_impl_opengl3.cpp
                include/imgui/imgui_tables.cpp
                include/imgui/imgui_widgets.cpp
                src/windowmanager/WindowManager.cpp 
                src/render/mesh.cpp 
                src/shader/shader.cpp 
                src/system/time.cpp 
                src/render/sprite.cpp 
                src/render/entity.cpp
                src/render/texture.cpp
                src/system/file.cpp
                src/render/transform.cpp
                src/render/camera.cpp
                src/editor/editorWM.cpp
                src/system/mouse.cpp
                src/editor/editor.cpp
                src/render/material.cpp
                src/render/framebuffer.cpp
                src/render/rendertexture.cpp
                src/editor/project.cpp
                src/render/level.cpp
                src/audio/audio.cpp
                src/audio/audiosource.cpp
                src/audio/audioclip.cpp
                src/audio/channelgroup.cpp
                engine.rc
                )

#add_link_options(-static -static-libgcc -static-libstdc++)


target_link_libraries(engine PUBLIC 
"${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a" 
"${CMAKE_CURRENT_SOURCE_DIR}/lib/fmod_vc.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/lib/fmodL_vc.lib"  
gdi32 opengl32)

