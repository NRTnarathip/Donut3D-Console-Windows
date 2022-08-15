#include <algorithm>
#include <iostream>
#include <windows.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
float m_time = 0.f;
void wait(DWORD millisecond)
{
    Sleep(millisecond);
}
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
float sdTorus(glm::vec3 p, glm::vec2 t)
{

    glm::vec2 q = {glm::length(glm::vec2(p.x, p.z)) - t.x, p.y};
    return glm::length(q) - t.y;
}
float getWorldMap(glm::vec3 point)
{
    auto pointTorus = point;
    auto angleTorus = glm::vec3(m_time * -10.f, m_time * -15.f, m_time * -20.f);
    auto transformTorus = rotateX(angleTorus.x) * rotateY(angleTorus.y) * rotateZ(angleTorus.z);
    pointTorus = pointTorus * transformTorus;
    float d = sdTorus(pointTorus, {2, 1.2f});
    return d;
}
glm::vec3 getNormal(glm::vec3 point)
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
    return normalize(normal);
}
float rayMarching(glm::vec3 rayOrigin, glm::vec3 rayDir)
{
    float totalDistanceTraveled = 0.0f;
    const int NUMBER_OF_STEPS = 32;
    const float MIN_HIT_DIST = 0.001f;
    const float MAX_TRACE_DIST = 1000.f;
    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        glm::vec3 current = rayOrigin + (totalDistanceTraveled * rayDir);
        float distCircle = getWorldMap(current);
        if (distCircle < MIN_HIT_DIST)
        {
            auto normal = getNormal(current);
            // auto lightPos = glm::vec3(0, 20, glm::sin(m_time) * 150);
            auto lightPos = glm::vec3(-5, 50, 30);

            auto lightDir = glm::normalize(lightPos - current);

            float diff = std::max(0.0f, glm::dot(normal, -lightDir));
            diff = std::max(diff, 0.2f);
            return diff;
        }

        if (totalDistanceTraveled > MAX_TRACE_DIST)
        {
            break;
        }

        totalDistanceTraveled += distCircle;
    }
    return 0.f;
}

int main()
{
    auto camera = glm::vec3(0, 0, -10);
    const unsigned int resX = 50;
    const unsigned int resY = 50;
    const unsigned int resTotal = resX * resY;
    std::string myAsciiDensity = ".:;-+=s#@S$&";
    uint32_t myAsciiDensityLen = myAsciiDensity.length();
    const uint32_t bufferLen = (resTotal + resTotal) + resY;
    char bufferRender[bufferLen];
    float bufferSDF[resX * resY];
    // added the two lines below
    memset(bufferRender, 10, bufferLen);
    uint32_t maxFPS = 144;
    float maxDeltaTime = 1.f / maxFPS;
    float totalTimeFPS = 0.f;
    uint32_t fps = 0;
    uint32_t totalFPS = 0;
    HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE); // handle to console window
    COORD C;
    C.X = 0;
    C.Y = 0;
    system("cls");
    while (true)
    {
        wait((int)std::round(maxDeltaTime * 1000));
        // wait(42);
        m_time += maxDeltaTime;
        totalTimeFPS += m_time;
        totalFPS++;
        if (totalTimeFPS > 1.f)
        {
            fps = totalFPS;

            totalFPS = 0;
            totalTimeFPS = 0.f;
        }
        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                int access = x + (y * resX);
                glm::vec2 uv = glm::vec2(x / (float)resX, y / (float)resY);
                uv = uv - glm::vec2(0.5f);
                auto rayDir = glm::vec3(uv, 1);
                auto rayOrigin = camera;
                auto d = rayMarching(rayOrigin, rayDir);
                bufferSDF[access] = d;
            }
        }
        uint32_t accBufferNow = 0;
        for (int y = 0; y < resY; y++)
        {
            for (int x = 0; x < resX; x++)
            {
                auto acc = x + (y * resX);
                float d = bufferSDF[acc];
                char charRender = ' ';
                if (d > 0.f)
                {
                    int charIndex = std::floor(d * myAsciiDensityLen);
                    charRender = myAsciiDensity[charIndex - 1];
                }
                bufferRender[++accBufferNow] = charRender;
                bufferRender[++accBufferNow] = ' ';
            }
            bufferRender[++accBufferNow] = '\n';
        }
        // define coordinates, set them to 0,0
        SetConsoleCursorPosition(hand, C);                           // set the cursor to 0,0
        DWORD written;                                               // helper flag, unused
        WriteConsole(hand, bufferRender, bufferLen, &written, NULL); // write the character contents of buff[]
    }
    return 0;
}