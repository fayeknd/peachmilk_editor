#include <iostream>
#include "editor/editorWM.hpp"

void HelloWorld() {
    std::cout << "Built " << __DATE__ << " " << __TIME__ << std::endl;
}

int main(){

    EditorWM wm;
    std::string buildTime =  std::string(WND_TITLE) + " (Built " __DATE__ + " " + __TIME__ + ")";
    wm.create(WND_W, WND_H, buildTime.c_str(), HelloWorld);

    return 0; 

}
