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
#include "Model.h"
#include "Light.h"
#include "Scene.h"
#include "Material.h"

#define EDGE_AA


#if defined _WIN32 || defined _WIN64
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

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
const int AA_SAMPLES = 4;
const glm::vec2 JITTER_MATRIX[AA_SAMPLES] = { glm::vec2(-0.25, 0.75), glm::vec2(0.75, 0.25), glm::vec2(-0.75, -0.25), glm::vec2(0.25, -0.75) };
SDL_Surface *screen;
int t;
vector<Triangle> triangles;
const float FOCAL_LENGTH = SCREEN_WIDTH / 2;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update(Scene &scene, Uint8 &lightSelected);

void Draw(Scene &scene);

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

	auto cubeScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cube.obj"), glm::vec3(0.0f, 0.0f, 0.0f)) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });
	
	auto bunnyScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/bunny.obj"), glm::vec3(0.0f, 0.0f, 0.0f)) },
		std::vector<Light> { 
			Light{ vec3(0.0f, 0.5f, -1.0f), 15.0f * vec3(1,1,1) },
			Light{ vec3(0.5f, 0.1f, 0.0f), 15.0f * vec3(1,1,1) }},
		Camera{ glm::vec3(0.0f, 0.1f, -0.15f), 0.0f, 0.0f, 0.0f });
	/*
	auto teapotScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/teapot.obj"), glm::vec3(-3.0f, 0.0f, 0.0f)),
			ModelInstance(Model("Resources/cube.obj"), glm::vec3(3.0f, 0.0f, 0.0f))
		},
		std::vector<Light> {
			Light{ vec3(3.0f, 2.0f, 0.0f), 100.0f * vec3(1,1,1) },
			Light{ vec3(-3.0f, 4.0f, 2.0f), 100.0f * vec3(1,1,1) },
			Light{ vec3(-3.0f, 4.0f, -2.0f), 30.0f * vec3(1,1,1) }},
		Camera{ glm::vec3(0.0f, 4.0f, -7.0f), 30.0f, 0.0f, 0.0f });*/

	Scene &scene = cubeScene;
	
	std::vector<Triangle> sceneTris = scene.ToTriangles();
	triangles.insert(triangles.end(), sceneTris.begin(), sceneTris.end());
	cout << "Loaded " << triangles.size() << " tris" << endl;

	while (NoQuitMessageSDL()) {
		Draw(scene);
		Update(scene, lightSelected);
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Update(Scene &scene, Uint8 &lightSelected) {
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms.\n";

	static float movementSpeed = 0.001;
	static float rotateSpeed = 0.01;

	auto keystate = SDL_GetKeyState(nullptr);
	if (keystate[SDLK_UP]) {
		scene.camera_.position.y += dt * movementSpeed;
	}
	if (keystate[SDLK_DOWN]) {
		scene.camera_.position.y -= dt * movementSpeed;
	}
	if (keystate[SDLK_LEFT]) {
		scene.camera_.position.x -= dt * movementSpeed;
	}
	if (keystate[SDLK_RIGHT]) {
		scene.camera_.position.x += dt * movementSpeed;
	}
	if (keystate[SDLK_w]) {
		scene.lights_[lightSelected].position += vec3(0.0f, 0.0f, movementSpeed * dt);
	}
	if (keystate[SDLK_s]) {
		scene.lights_[lightSelected].position += vec3(0.0f, 0.0f, -movementSpeed * dt);
	}
	if (keystate[SDLK_a]) {
		scene.lights_[lightSelected].position += vec3(-movementSpeed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_d]) {
		scene.lights_[lightSelected].position += vec3(movementSpeed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_q]) {
		scene.lights_[lightSelected].position += vec3(0.0f, movementSpeed * dt, 0.0f);
	}
	if (keystate[SDLK_e]) {
		scene.lights_[lightSelected].position += vec3(0.0f, -movementSpeed * dt, 0.0f);
	}
	if (keystate[SDLK_n]) {
		if (scene.lights_.size() < 6) {
			AddLight(vec3(0.0f, 0.0f, 0.0f), 10.0f * vec3(1.0f, 1.0f, 1.0f), scene.lights_);
			lightSelected = static_cast<Uint8>(scene.lights_.size()) - 1;
		}
	}

	if (keystate[SDLK_j])
	{
		scene.camera_.yaw -= dt * rotateSpeed;
	}
	if (keystate[SDLK_l])
	{
		scene.camera_.yaw += dt * rotateSpeed;
	}
	if (keystate[SDLK_i])
	{
		scene.camera_.pitch -= dt * rotateSpeed;
	}
	if (keystate[SDLK_k])
	{
		scene.camera_.pitch += dt * rotateSpeed;
	}
	if (keystate[SDLK_u])
	{
		scene.camera_.roll -= dt * rotateSpeed;
	}
	if (keystate[SDLK_o])
	{
		scene.camera_.roll += dt * rotateSpeed;
	}

	if (keystate[SDLK_1] && scene.lights_.size() > 0) {
		lightSelected = 0;
	}
	if (keystate[SDLK_2] && scene.lights_.size() > 1) {
		lightSelected = 1;
	}
	if (keystate[SDLK_3] && scene.lights_.size() > 2) {
		lightSelected = 2;
	}
	if (keystate[SDLK_4] && scene.lights_.size() > 3) {
		lightSelected = 3;
	}
	if (keystate[SDLK_5] && scene.lights_.size() > 4) {
		lightSelected = 4;
	}
	if (keystate[SDLK_6] && scene.lights_.size() > 5) {
		lightSelected = 5;
	}
}

void Draw(Scene &scene) {
	//SDL_FillRect( screen, 0, 0 );

	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	// Paint every pixel on the screen
#pragma omp parallel for
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {

			vec3 colour(0.0f, 0.0f, 0.0f);

			// Record the indices of the triangles at each sample point. If we hit the same triangle twice we can re-use the surface colour
			int sampleTriangleIndices[AA_SAMPLES];
			std::fill(std::begin(sampleTriangleIndices), std::end(sampleTriangleIndices), -1);
			vec3 sampleTriangleColours[AA_SAMPLES];

			for (int sample = 0; sample < AA_SAMPLES; sample++) {
				// Compute the corresponding camera-space co-ordinates (u,v,f) for this point on the screen
				float u = x - SCREEN_WIDTH / 2 + JITTER_MATRIX[sample].x;
				float v = (SCREEN_HEIGHT - y) - SCREEN_HEIGHT / 2 + JITTER_MATRIX[sample].y;
				vec3 d(u, v, FOCAL_LENGTH);

				// Rotate the direction of the camera->screen ray according to the current pitch, roll and yaw
				d = glm::rotate(d, glm::radians(scene.camera_.pitch), vec3(1.0f, 0.0f, 0.0f));
				d = glm::rotate(d, glm::radians(scene.camera_.yaw), vec3(0.0f, 1.0f, 0.0f));
				d = glm::rotate(d, glm::radians(scene.camera_.roll), vec3(0.0f, 0.0f, 1.0f));


				// Find an intersection of this ray with the model, if exists
				Intersection maybeIntersection;
				bool hasIntersection = ClosestIntersection(scene.camera_.position, d, triangles, triangles.size(), maybeIntersection);

#ifdef EDGE_AA
				if (hasIntersection) {
					bool alreadySampled = false;
					for (int i = 0; i < AA_SAMPLES; i++)
					{
						if (sampleTriangleIndices[i] == maybeIntersection.triangleIndex)
						{
							colour += sampleTriangleColours[i];
							alreadySampled = true;
							break;
						}
					}

					if (!alreadySampled)
					{
						vec3 triangleColour = SurfaceColour(maybeIntersection, scene.lights_, d);
						colour += triangleColour;
						sampleTriangleIndices[sample] = maybeIntersection.triangleIndex;
						sampleTriangleColours[sample] = triangleColour;
					}
				}
#else
				if (hasIntersection) {
					colour += SurfaceColour(maybeIntersection, lights, d);
				}
#endif

			}
			PutPixelSDL(screen, x, y, colour / float(AA_SAMPLES));
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
	glm::vec3 lightIntensity(0.0f, 0.0f, 0.0f);
	glm::vec3 specularIntensity(0.0f, 0.0f, 0.0f);
	for (auto light : lights) {

		// Determine whether the shadow ray (intersection->light) intersects with another triangle
		vec3 shadowRay = light.position - intersection.position;
		float lightDistance = glm::length(shadowRay);
		Intersection shadowRayIntersection;
		bool hasIntersection = ClosestIntersection(intersection.position, shadowRay, triangles, intersection.triangleIndex, shadowRayIntersection);

		// If an intersection was found, and it is between the intersection and the light, this light does not reach the intersection
		if (hasIntersection) {
			float shadowRayIntersectionDistance = glm::length(intersection.position - shadowRayIntersection.position);
			if (shadowRayIntersectionDistance <= lightDistance)
				continue;
		}

		// Otherwise, compute the light's power per unit area at the intersection
		vec3 B = (light.color / static_cast<float>(4 * lightDistance * lightDistance * M_PI));
		// Compute a normalising factor based on the angle between the shadow ray and the surface normal
		float dp = abs(glm::dot(glm::normalize(triangles[intersection.triangleIndex].normal), glm::normalize(shadowRay)));
		lightIntensity += B * glm::max(dp, 0.0f);

		// Compute direction of 'ideal' reflection from light source
		vec3 idealReflection = glm::normalize(glm::reflect(shadowRay, triangles[intersection.triangleIndex].normal));
		// Project actual incident ray onto reflection and use Phong to calculate specular intensity
		specularIntensity += B * static_cast<float>(std::pow(std::max(0.0f, glm::dot(idealReflection, glm::normalize(incidentRay))), 100));

	}
	return lightIntensity * Kdiffuse + specularIntensity * Kspecular;
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
