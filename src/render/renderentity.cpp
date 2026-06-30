#include "renderentity.hpp"
#include "entity.hpp"
#include "sprite.hpp"
#include "level.hpp"
#include "text/text.hpp"

void RenderEntity::orderEntitiesByZ(bool flip) {
    quicksort(0, s_allRenderEntities.size() - 1, flip);
}

float RenderEntity::partition(int left, int right, bool flip) {
    float pivot = dynamic_cast<ScriptableEntity*>(s_allRenderEntities[right])->transform.getGlobalPosition().z;
    int sortedIndex = left - 1;

    for (int j = left; j < right; j++) {
        ScriptableEntity* entity = dynamic_cast<ScriptableEntity*>(s_allRenderEntities[j]);
        if (flip) {

            if (entity->transform.getGlobalPosition().z > pivot) {
                sortedIndex++;
                std::swap(s_allRenderEntities[sortedIndex], s_allRenderEntities[j]);
            }
        }
        else if (entity->transform.getGlobalPosition().z < pivot) {
            sortedIndex++;
            std::swap(s_allRenderEntities[sortedIndex], s_allRenderEntities[j]);
        }
    }

    std::swap(s_allRenderEntities[sortedIndex + 1], s_allRenderEntities[right]);
    return sortedIndex + 1;
}

void RenderEntity::quicksort(int left, int right, bool flip) {
    if (left < right) {
        int pivot = partition(left, right, flip);

        quicksort(left, pivot - 1, flip);
        quicksort(pivot + 1, right, flip);
    }
}

void RenderEntity::drawEntites() {
    ScriptableEntity::updateTransforms();
    for (int i = s_allRenderEntities.size() - 1; i >= 0; --i) {
        Sprite* spr = dynamic_cast<Sprite*>(s_allRenderEntities[i]);
        if (spr) {
            if (spr->preDraw()) spr->draw();
        }
        else {
            TextRenderer* tr = dynamic_cast<TextRenderer*>(s_allRenderEntities[i]);
            if (tr) {
                if (tr->preDraw()) tr->draw();
            }
        }
    }
}

void RenderEntity::oncreate_positionZ() {
    ScriptableEntity* ent = dynamic_cast<ScriptableEntity*>(this);
    if (RenderEntity::s_allRenderEntities.size() == 1) {
        ent->transform.setGlobalPositionZ(-255);
    }
    else {
        ent->transform.setGlobalPositionZ(dynamic_cast<ScriptableEntity*>(RenderEntity::s_allRenderEntities[RenderEntity::s_allRenderEntities.size() - 2])->transform.getGlobalPosition().z + 1);
    }
}
