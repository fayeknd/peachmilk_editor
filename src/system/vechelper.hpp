#pragma once
#include "../headers.h"

namespace vecmath {

    static double length(glm::vec3 & v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

};