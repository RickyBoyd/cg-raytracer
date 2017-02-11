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
#include <algorithm>
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

vec3 DirectLight(const Intersection &i, vector<Light> &lights, vec3 incidentRay);

vec3 IndirectLight();

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights, vec3 incidentRay);

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
	cout << "Render time: " << dt << " ms.\n";

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

	// Paint every pixel on the screen
#pragma omp parallel for
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {

			// Compute the corresponding camera-space co-ordinates (u,v,f) for this point on the screen
			int u = x - SCREEN_WIDTH / 2;
			int v = (SCREEN_HEIGHT - y) - SCREEN_HEIGHT / 2;
			vec3 d(u, v, f);

			// Rotate the direction of the camera->screen ray according to the current pitch, roll and yaw
			d = glm::rotate(d, glm::radians(pitchYawRoll.x), vec3(1.0f, 0.0f, 0.0f));
			d = glm::rotate(d, glm::radians(pitchYawRoll.y), vec3(0.0f, 1.0f, 0.0f));
			d = glm::rotate(d, glm::radians(pitchYawRoll.z), vec3(0.0f, 0.0f, 1.0f));

			// Find an intersection of this ray with the model, if exists
			Intersection maybeIntersection;
			bool hasIntersection = ClosestIntersection(cameraPos, d, triangles, triangles.size(), maybeIntersection);
			if (hasIntersection) {
				vec3 color = SurfaceColour(maybeIntersection, lights, d);
				PutPixelSDL(screen, x, y, color);
			}
			else {
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

	// Check each triangle for an intersection using the Moller-Trumbore algorithm
	// TODO: optimise this process using bounding boxes
	for (int i = 0; i < triangles.size(); i++) {

		if (i == currentTriangleIndex) continue;

		Triangle triangle = triangles[i];
		vec3 e1 = triangle.v1 - triangle.v0;
		vec3 e2 = triangle.v2 - triangle.v0;
		vec3 pvec = glm::cross(dir, e2);
		float det = glm::dot(e1, pvec);

		if (fabs(det) < 0.000000000001) continue;

		float invDet = 1.0f / det;

		vec3 tvec = start - triangle.v0;
		float u = glm::dot(tvec, pvec) * invDet;
		if (u < 0) continue;

		vec3 qvec = glm::cross(tvec, e1);
		float v = glm::dot(dir, qvec) * invDet;
		if (v < 0 || u + v > 1) continue;

		float t = glm::dot(e2, qvec) * invDet;

		if (t > 0 && t < closestIntersection.distance) {
			closestIntersection.position = triangle.v0 + u * e1 + v * e2;
			closestIntersection.distance = t;
			closestIntersection.triangleIndex = i;
		}
	}
	return closestIntersection.distance != std::numeric_limits<float>::max();
}

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights, vec3 incidentRay) {
	return triangles[i.triangleIndex].color * (DirectLight(i, lights, incidentRay) + IndirectLight());
}

vec3 IndirectLight()
{
	return 0.2f*vec3(1, 1, 1);
}

// Compute the light intensity at an intersection resulting from direct light
vec3 DirectLight(const Intersection &intersection, vector<Light> &lights, vec3 incidentRay) {
	float Kdiffuse = 0.8f;
	float Kspecular = 0.5f;

	// Sum the light intensity over all lights in the scene
	glm::vec3 diffuseIntensity(0.0f, 0.0f, 0.0f);
	glm::vec3 specularIntensity(0.0f, 0.0f, 0.0f);
	for (auto light : lights) {
		
		// Determine whether the shadow ray (intersection->light) intersects with another triangle
		vec3 shadowRay = light.position - intersection.position;
		float lightDistance = glm::length(shadowRay);
		Intersection shadowRayIntersection;
		bool hasIntersection = ClosestIntersection(intersection.position, shadowRay, triangles, intersection.triangleIndex, shadowRayIntersection);

		// If an intersection was found, and it is between the intersection and the light, this light does not reach the intersection
		if (hasIntersection && shadowRayIntersection.distance < lightDistance) {
			continue;
		}

		// Otherwise, compute the light's power per unit area at the intersection
		vec3 B = (light.color / static_cast<float>(4 * lightDistance * lightDistance * M_PI));
		// Compute a normalising factor based on the angle between the shadow ray and the surface normal
		float dp = glm::dot(glm::normalize(triangles[intersection.triangleIndex].normal), glm::normalize(shadowRay));

		diffuseIntensity += B * glm::max(dp, 0.0f);

		// Compute direction of 'ideal' reflection from light source
		vec3 idealReflection = glm::normalize(glm::reflect(shadowRay, triangles[intersection.triangleIndex].normal));
		// Project actual incident ray onto reflection and use Phong to calculate specular intensity
		specularIntensity += B * std::pow(std::max(0.0f, glm::dot(idealReflection, glm::normalize(incidentRay))), 100);
	}
	return diffuseIntensity * Kdiffuse + specularIntensity * Kspecular;
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
