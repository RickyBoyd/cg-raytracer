
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <limits>


const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float f = SCREEN_WIDTH / 2.0f;
extern SDL_Surface *screen;
extern int t;
const glm::vec3 cameraPos(0.0f, 0.0f, -2.0f);
const glm::vec3 black(0.0, 0.0, 0.0);

struct Intersection {
    glm::vec3 position;
    float distance;
    int triangleIndex;
};

void Update();

void Draw(std::vector<Triangle> &triangles);

void Interpolate(float a, float b, std::vector<float> &result);

void Interpolate(glm::vec3 a, glm::vec3 b, std::vector<glm::vec3> &result);

bool ClosestIntersection(glm::vec3 start, glm::vec3 dir, const std::vector<Triangle> &triangles,
                         Intersection &closestIntersection);

#endif