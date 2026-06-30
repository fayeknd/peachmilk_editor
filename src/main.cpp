#include <iostream>
#include "editor/editorWM.hpp"
#include "audio/audio.hpp"

// sometimes i think i'm pretty useless at programming.
// i know i'm not the best and i don't make the most consistent
// or efficient or even just smart choices but i don't need to.
// i'm making something i want to make and i'm proud to be able
// to even do that. this is NOT the best code but in a way i'm
// attached to it. this is huge and i've watched it grow and change
// and shape into something i love. the wheel has already been invented,
// i know, but for me, hacking together a new bumpy wheel for my own car
// is not about efficiency or challenging other wheels,
// it's about doing it because you want to.
// i wanted to make this so i did. and it was harder.
//
// this is _really_ hard to do, but at the same time this is our thing
// that we made and i'm proud to say i participated and a large language
// model didn't.




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
