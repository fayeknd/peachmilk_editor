#pragma once
#include "entity.hpp"
#include "../headers.h"

class RenderEntity {
private:
    static float partition(int left, int righ, bool flip);
    static void quicksort(int left, int right, bool flip);
public:

    static void orderEntitiesByZ(bool flip);

    static inline std::vector<RenderEntity*> s_allRenderEntities;
    static void drawEntites();
    virtual bool draw() {return true;};
    virtual void oncreate_positionZ();
};
