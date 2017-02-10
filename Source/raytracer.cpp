#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>
//#include <omp.h>


#ifndef unix
extern "C" {
	FILE __iob_func[3] = { stdin, stdout,*stderr };
}
extern "C" {
	FILE __imp_fprintf[3] = { stdin, stdout,*stderr };
}
#endif

using namespace std;
using glm::vec3;
using glm::mat3;

struct Intersection {
	vec3 position;
	float distance;
	int triangleIndex;
};

struct Light {
	vec3 position;
	vec3 color;
};

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface *screen;
int t;
vector<Triangle> triangles;
float f = SCREEN_WIDTH / 2;
const vec3 BLACK(0.0f, 0.0f, 0.0f);

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update(glm::vec3 &cameraPos, vector<Light> &lights, Uint8 *lightSelected, vec3 &pitchYawRoll);

void Draw(glm::vec3 &cameraPos, vec3 pitchYawRoll, vector<Light> &lights);

void Interpolate(float a, float b, vector<float> &result);

bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle> &triangles, int currentTriangleIndex,
                         Intersection &closestIntersection);

vec3 DirectLight(const Intersection &i, vector<Light> &lights);

vec3 IndirectLight();

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights);

void AddLight(vec3 pos, vec3 color, vector<Light> &lights);


int main(int argc, char *argv[]) {
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();    // Set start value for timer.

	Uint8 lightSelected = 0;

	LoadTestModel(triangles);
	cout << "Loaded " << triangles.size() << " tris" << endl;

	vec3 cameraPos(0.0f, 0.0f, -2.0f);
	vec3 pitchYawRoll = vec3(0.0f, 0.0f, 0.0f);

	vector<Light> lights;
	AddLight(vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1, 1, 1), lights);

	while (NoQuitMessageSDL()) {
		Draw(cameraPos, pitchYawRoll, lights);
		Update(cameraPos, lights, &lightSelected, pitchYawRoll);
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Update(glm::vec3 &cameraPos, vector<Light> &lights, Uint8 *lightSelected, vec3 &pitchYawRoll) {
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	static float movementSpeed = 0.001;
	static float rotateSpeed = 0.01;

	Uint8 *keystate = SDL_GetKeyState(nullptr);
	if (keystate[SDLK_UP]) {
		cameraPos.y += dt * movementSpeed;
	}
	if (keystate[SDLK_DOWN]) {
		cameraPos.y -= dt * movementSpeed;
	}
	if (keystate[SDLK_LEFT]) {
		cameraPos.x -= dt * movementSpeed;
	}
	if (keystate[SDLK_RIGHT]) {
		cameraPos.x += dt * movementSpeed;
	}
	if (keystate[SDLK_w]) {
		lights[*lightSelected].position += vec3(0.0f, 0.0f, movementSpeed * dt);
	}
	if (keystate[SDLK_s]) {
		lights[*lightSelected].position += vec3(0.0f, 0.0f, -movementSpeed * dt);
	}
	if (keystate[SDLK_a]) {
		lights[*lightSelected].position += vec3(-movementSpeed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_d]) {
		lights[*lightSelected].position += vec3(movementSpeed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_q]) {
		lights[*lightSelected].position += vec3(0.0f, movementSpeed * dt, 0.0f);
	}
	if (keystate[SDLK_e]) {
		lights[*lightSelected].position += vec3(0.0f, -movementSpeed * dt, 0.0f);
	}
	if (keystate[SDLK_n]) {
		if (lights.size() < 6) {
			AddLight(vec3(0.0f, 0.0f, 0.0f), 10.0f * vec3(1.0f, 1.0f, 1.0f), lights);
			*lightSelected = static_cast<Uint8>(lights.size()) - 1;
		}
	}

	if (keystate[SDLK_j])
	{
		pitchYawRoll.y -= dt * rotateSpeed;
	}
	if (keystate[SDLK_l])
	{
		pitchYawRoll.y += dt * rotateSpeed;
	}
	if (keystate[SDLK_i])
	{
		pitchYawRoll.x -= dt * rotateSpeed;
	}
	if (keystate[SDLK_k])
	{
		pitchYawRoll.x += dt * rotateSpeed;
	}
	if (keystate[SDLK_u])
	{
		pitchYawRoll.z -= dt * rotateSpeed;
	}
	if (keystate[SDLK_o])
	{
		pitchYawRoll.z += dt * rotateSpeed;
	}

	if (keystate[SDLK_1] && lights.size() > 0) {
		*lightSelected = 0;
	}
	if (keystate[SDLK_2] && lights.size() > 1) {
		*lightSelected = 1;
	}
	if (keystate[SDLK_3] && lights.size() > 2) {
		*lightSelected = 2;
	}
	if (keystate[SDLK_4] && lights.size() > 3) {
		*lightSelected = 3;
	}
	if (keystate[SDLK_5] && lights.size() > 4) {
		*lightSelected = 4;
	}
	if (keystate[SDLK_6] && lights.size() > 5) {
		*lightSelected = 5;
	}
}

void Draw(glm::vec3 &cameraPos, vec3 pitchYawRoll, vector<Light> &lights) {
	//SDL_FillRect( screen, 0, 0 );

	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

#pragma omp parallel for
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			int u = x - SCREEN_WIDTH / 2;
			int v = (SCREEN_HEIGHT - y) - SCREEN_HEIGHT / 2;
			vec3 d(u, v, f);
			d = glm::rotate(d, glm::radians(pitchYawRoll.x), vec3(1.0f, 0.0f, 0.0f));
			d = glm::rotate(d, glm::radians(pitchYawRoll.y), vec3(0.0f, 1.0f, 0.0f));
			d = glm::rotate(d, glm::radians(pitchYawRoll.z), vec3(0.0f, 0.0f, 1.0f));
			Intersection maybeIntersection;
			bool hasIntersection = ClosestIntersection(cameraPos, d, triangles, triangles.size(), maybeIntersection);
			if (hasIntersection) {
				vec3 color = SurfaceColour(maybeIntersection, lights);
				PutPixelSDL(screen, x, y, color);
			}
			else {
				//cout << " NO intersection at (" << x <<"," << y << ")" << endl;
				PutPixelSDL(screen, x, y, BLACK);
			}
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/// <summary>Find the triangle intersected first by the ray from start in direction dir.</summary>
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle> &triangles, int currentTriangleIndex, Intersection &closestIntersection) {
	// Initialise the closest intersection to infinitely far away
	closestIntersection.distance = std::numeric_limits<float>::max();

	// Check each triangle for an intersection
	// TODO: optimise this process using bounding boxes
	for (int i = 0; i < triangles.size(); i++) {

		// TODO: optimise this computation using Cramer's rule
		Triangle triangle = triangles[i];
		vec3 e1 = triangle.v1 - triangle.v0;
		vec3 e2 = triangle.v2 - triangle.v0;
		vec3 b = start - triangle.v0;
		vec3 unitDir = glm::normalize(dir);
		mat3 A(-unitDir, e1, e2);
		vec3 x = inverse(A) * b;

		// If there is an intersection with this triangle
		if ((x.x >= 0) && (x.y >= 0) && (x.z >= 0) && (x.y + x.z <= 1))
		{
			// And if the intersection is closer
			if (closestIntersection.distance > x.x) {
				// Ignore intersections with the current triangle
				if (i == currentTriangleIndex) continue;

				// Record the newly-found closest intersection
				closestIntersection.position = start + x.x * dir;
				closestIntersection.distance = x.x;
				closestIntersection.triangleIndex = i;
			}

		}

	}
	return closestIntersection.distance != std::numeric_limits<float>::max();
}

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights) {
	return triangles[i.triangleIndex].color * (DirectLight(i, lights) + IndirectLight());
}

vec3 IndirectLight()
{
	return 0.2f*vec3(1, 1, 1);
}

vec3 DirectLight(const Intersection &intersection, vector<Light> &lights) {
	glm::vec3 lightIntensity(0.0f, 0.0f, 0.0f);

	//Cast ray with all light sources and check if closest intersection is before the light
	Intersection shadowRayIntersection;

	for (int i = 0; i < lights.size(); i++) {
		vec3 shadowRay = lights[i].position - intersection.position;
		bool isIntersection = ClosestIntersection(intersection.position, shadowRay, triangles, intersection.triangleIndex,
		                                          shadowRayIntersection);
		vec3 intersectVec = intersection.position - shadowRayIntersection.position;
		float r = glm::length(shadowRay);
		float intersectDis = glm::length(intersectVec);
		if (!isIntersection || intersectDis > r)
		{
			vec3 B = (lights[i].color / (float)(4 * r * r * M_PI));
			float dp = glm::dot(glm::normalize(triangles[intersection.triangleIndex].normal), glm::normalize(shadowRay));
			lightIntensity += B * glm::max(dp, 0.0f);
		}
	}
	return lightIntensity;
}


void Interpolate(float a, float b, vector<float> &result) {
	if (result.size() == 0) return;
	if (result.size() == 1) {
		result[0] = (a + b) / 2;
		return;
	}
	else {
		float diff = b - a;
		float step = diff / (result.size() - 1);
		result[0] = a;
		for (int i = 1; i < result.size(); i++) {
			result[i] = result[i - 1] + step;
		}
	}
}

void AddLight(vec3 pos, vec3 color, vector<Light> &lights) {
	Light light = { pos, color };
	lights.push_back(light);
}
