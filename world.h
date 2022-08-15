#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include "gameobject.h"
#include "glm/glm.hpp"

class World
{
public:
    static World *instance;
    World()
    {
        instance = this;
    }
    static World *GetInstance()
    {
        return instance;
    }
    std::vector<GameObject *> m_gameObjects;
    uint32_t m_entityNewCounter = 0;
    void update(float dt)
    {
        for (auto obj : m_gameObjects)
        {
            obj->update(dt);
        }
    }
    glm::vec3 getWorldMapNormal(glm::vec3 point)
    {
        const float smallStep = 0.001f;
        float mapX = getWorldMap(point + glm::vec3(smallStep, 0, 0));
        float mapY = getWorldMap(point + glm::vec3(0, smallStep, 0));
        float mapZ = getWorldMap(point + glm::vec3(0, 0, smallStep));
        float mapX2 = getWorldMap(point - glm::vec3(smallStep, 0, 0));
        float mapY2 = getWorldMap(point - glm::vec3(0, smallStep, 0));
        float mapZ2 = getWorldMap(point - glm::vec3(0, 0, smallStep));
        float gradient_x = mapX - mapX2;
        float gradient_y = mapY - mapY2;
        float gradient_z = mapZ - mapZ2;

        glm::vec3 normal = glm::vec3(gradient_x, gradient_y, gradient_z);
        return glm::normalize(normal);
    }
    float getWorldMap(glm::vec3 point)
    {
        float d = FLT_MAX;
        for (auto obj : m_gameObjects)
        {
            d = std::min(d, obj->getSDF(point));
        }
        return d;
    }
    GameObject *newGameObject(const char *objectName)
    {
        GameObject *obj = new GameObject();

        m_entityNewCounter++;
        obj->entity = m_entityNewCounter;

        m_gameObjects.push_back(obj);
        return obj;
    }

    bool deleteGameObject(uint32_t entity)
    {
        for (auto it = std::begin(m_gameObjects); it != std::end(m_gameObjects); ++it)
        {
            auto obj = (*it);
            if (obj->entity == entity)
            {
                m_gameObjects.erase(it);
                delete obj;
                return true;
            }
        }
        return false;
    }
};
