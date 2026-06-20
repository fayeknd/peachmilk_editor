#include <iostream>
#include "editor/editorWM.hpp"
#include "system/audio.hpp"

void _audio_err_callback() {
    exit(-1);
}

void HelloWorld() {
    std::cout << "Built " << __DATE__ << " " << __TIME__ << std::endl;
}


int main(){

    AudioManager::get().setErrCallback(_audio_err_callback);
    AudioManager::get().create();

    EditorWM wm;
    std::string buildTime =  std::string(WND_TITLE) + " (Built " __DATE__ + " " + __TIME__ + ")";
    wm.create(WND_W, WND_H, buildTime.c_str(), HelloWorld);

    return 0; 

}
