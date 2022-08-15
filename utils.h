#pragma once
#include "glm/glm.hpp"
#include <windows.h>

glm::mat3 rotateX(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    return glm::mat3({1, 0, 0}, {0, c, -s}, {0, s, c});
}
glm::mat3 rotateY(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    return glm::mat3({c, 0, s}, {0, 1, 0}, {-s, 0, c});
}
glm::mat3 rotateZ(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
    return glm::mat3({c, -s, 0}, {s, c, 0}, {0, 0, 1});
}
void wait(DWORD millisecond)
{
    Sleep(millisecond);
}
float sdTorus(glm::vec3 p, glm::vec2 t)
{

    glm::vec2 q = {glm::length(glm::vec2(p.x, p.z)) - t.x, p.y};
    return glm::length(q) - t.y;
}