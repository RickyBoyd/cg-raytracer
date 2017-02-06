#include "raytracer.h"

int main(int argc, char *argv[]) {
    screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
    t = SDL_GetTicks();    // Set start value for timer.

    std::vector<Triangle> triangles;
    LoadTestModel(triangles);
    std::cout << triangles.size() << std::endl;

    while (NoQuitMessageSDL()) {
        Draw(triangles);
    }

    SDL_SaveBMP(screen, "screenshot.bmp");
    return 0;
}