#pragma once

#include <iostream>
#include "glm/glm.hpp"
#include <functional>

typedef uint32_t entity_t;

class GameObject
{
public:
    entity_t entity = 0;
    std::string name = "New Game Object";
    glm::vec3 position;
    glm::vec3 scale = {1, 1, 1};
    glm::vec3 rotation; // unit degree angle
    std::function<void(float)> funcOnUpdate = nullptr;
    std::function<float(glm::vec3)> funcOnGetSDF = nullptr;
    void onUpdate(std::function<void(float)> func)
    {
        funcOnUpdate = func;
    }
    void onGetSDF(std::function<float(glm::vec3)> func)
    {
        funcOnGetSDF = func;
    }
    float getSDF(glm::vec3 point)
    {
        if (!funcOnGetSDF)
            return FLT_MAX;

        float d = funcOnGetSDF(point);
        return d;
    }
    void update(float dt)
    {
        if (funcOnUpdate)
        {
            funcOnUpdate(dt);
        }
    };
};