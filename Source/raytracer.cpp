#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "Triangle.h"
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
#define RAY_DEPTH 4

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

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int AA_SAMPLES = 4;
const glm::vec2 JITTER_MATRIX[AA_SAMPLES] = { glm::vec2(-0.25, 0.75), glm::vec2(0.75, 0.25), glm::vec2(-0.75, -0.25), glm::vec2(0.25, -0.75) };
SDL_Surface *screen;
int t;
const float FOCAL_LENGTH = SCREEN_WIDTH / 2;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update(Scene &scene, Uint8 &lightSelected);

void Draw(Scene &scene, const vector<Triangle> &triangles);

void Interpolate(float a, float b, vector<float> &result);

bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle> &triangles, int currentTriangleIndex,
	Intersection &closestIntersection);

vec3 Trace(vec3 startPos, vec3 incidentRay, vector<Light> lights, const vector<Triangle> &triangles, int triangleIndex, int depth);

float Fresnel(float c, float n);
//void Fresnel(const vec3 &I, const vec3 &N, const float &ior, float &kr) ;
vec3 DirectLight(const Intersection &i, vector<Light> &lights, const vector<Triangle> &triangles, vec3 incidentRay);

vec3 IndirectLight();

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights, const vector<Triangle> &triangles, vec3 incidentRay);

void AddLight(vec3 pos, vec3 color, vector<Light> &lights);

bool Refract(vec3 d, vec3 normal, float n, vec3 &t);

vec3 Reflect(vec3 d, vec3 normal);

int main(int argc, char *argv[]) {
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();    // Set start value for timer.

	Uint8 lightSelected = 0;

	auto cubeScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cube.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto cornellBoxScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box.obj")) },
		std::vector<Light> { Light { vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera { glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto cornellBoxTransparentScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box_transparency.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto bunnyBoxScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/cornell_box_empty.obj")),
			ModelInstance(Model("Resources/bunny_transparent.obj"), glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(12.0f, 12.0f, 12.0f)) 
		},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

#ifdef IMPORT_COMPLEX_MODELS
	auto bunnyScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/bunny.obj"), glm::vec3(0.0f, 0.0f, 0.0f)) },
		std::vector<Light> {
		Light{ vec3(0.0f, 0.5f, -1.0f), 15.0f * vec3(1,1,1) },
			Light{ vec3(0.5f, 0.1f, 0.0f), 15.0f * vec3(1,1,1) }},
		Camera{ glm::vec3(0.0f, 0.1f, -0.15f), 0.0f, 0.0f, 0.0f });
	
	auto teapotScene = Scene(
		std::vector<ModelInstance> {
			ModelInstance(Model("Resources/teapot.obj"), glm::vec3(-3.0f, 0.0f, 0.0f)),
			ModelInstance(Model("Resources/cube.obj"), glm::vec3(3.0f, 0.0f, 0.0f))
		},
		std::vector<Light> {
			Light{ vec3(3.0f, 2.0f, 0.0f), 100.0f * vec3(1,1,1) },
			Light{ vec3(-3.0f, 4.0f, 2.0f), 100.0f * vec3(1,1,1) },
			Light{ vec3(-3.0f, 4.0f, -2.0f), 30.0f * vec3(1,1,1) }},
		Camera{ glm::vec3(0.0f, 4.0f, -7.0f), 30.0f, 0.0f, 0.0f });
#endif

	Scene &scene = bunnyBoxScene;

	std::vector<Triangle> sceneTris = scene.ToTriangles();
	cout << "Loaded " << sceneTris.size() << " tris" << endl;

	// <<<<<<< HEAD
	// 	vec3 cameraPos(-0.2f, -0.2f, -2.0f);
	// 	vec3 pitchYawRoll = vec3(0.0f, 0.0f, 0.0f);

	// 	vector<Light> lights;
	// 	AddLight(vec3(-0.3f, 0.2f, -0.7f), 15.0f * vec3(1, 1, 1), lights);

	// =======
	// >>>>>>> master
	while (NoQuitMessageSDL()) {
		Draw(scene, sceneTris);
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

void Draw(Scene &scene, const vector<Triangle> &triangles) {
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

#ifdef EDGE_AA

				//bool alreadySampled = false;
				// for (int i = 0; i < AA_SAMPLES; i++)
				// {
				// 	if (sampleTriangleIndices[i] == maybeIntersection.triangleIndex)
				// 	{
				// 		colour += sampleTriangleColours[i];
				// 		alreadySampled = true;
				// 		break;
				// 	}
				// }

				//if (!alreadySampled)
				//{
				vec3 triangleColour = Trace(scene.camera_.position, d, scene.lights_, triangles, triangles.size(), 0);
				colour += triangleColour;
				//sampleTriangleIndices[sample] = maybeIntersection.triangleIndex;
				sampleTriangleColours[sample] = triangleColour;
				//}
#else
				colour += Trace(cameraPos, d, lights, triangles.size(), 0);
#endif

			}
			PutPixelSDL(screen, x, y, colour / float(AA_SAMPLES));
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

vec3 Trace(vec3 startPos, vec3 incidentRay, vector<Light> lights, const vector<Triangle> &triangles, int triangleIndex, int depth)
{
	// Find an intersection of this ray with the model, if exists
	float bias = 1e-3;
	Intersection maybeIntersection;
	bool hasIntersection = ClosestIntersection(startPos, incidentRay, triangles, triangleIndex, maybeIntersection);
	if (!hasIntersection)
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
	Triangle triangle = triangles[maybeIntersection.triangleIndex];
	if (triangle.transparency_ > 0.001f && depth <= RAY_DEPTH)
	{
		vec3 normal = triangles[maybeIntersection.triangleIndex].normal;
		float c;
		float refractiveIndex = triangle.refractive_index_;

		vec3 refractionRay(0.0f, 0.0f, 0.0f);
		normal = glm::normalize(normal);
		incidentRay = glm::normalize(incidentRay);

		vec3 reflectionRay = glm::reflect(incidentRay, normal);
		vec3 reflectionColor = Trace(maybeIntersection.position, reflectionRay, lights, triangles, maybeIntersection.triangleIndex, depth + 1);

		vec3 zero(0.0f, 0.0f, 0.0f);
		bool inside = glm::dot(incidentRay, normal) < 0;
		if (glm::dot(incidentRay, normal) < 0)
		{
			//bool refractionOccurs = Refract( incidentRay, normal, refractiveIndex, refractionRay);

			refractionRay = glm::refract(incidentRay, normal, refractiveIndex);

			c = -1.0f*glm::dot(incidentRay, normal);
		}
		else
		{
			//bool refractionOccurs = Refract( incidentRay, -1.0f*normal, 1.0f/refractiveIndex, refractionRay);
			refractionRay = glm::refract(incidentRay, -1.0f*normal, 1.0f / refractiveIndex);

			if (zero != refractionRay)
			{
				c = glm::dot(refractionRay, normal);
			}
			else
			{
				return reflectionColor;
			}
			//c = -1.0f*glm::dot( incidentRay,  normal);
		}
		//float R;
		//Fresnel(incidentRay, normal, refractiveIndex, R) ;
		float R = Fresnel(c, refractiveIndex);
		vec3 refractionColor = Trace(maybeIntersection.position, refractionRay, lights, triangles, maybeIntersection.triangleIndex, depth + 1);
		return (1.0f - R)*refractionColor + R*reflectionColor;
	}
	else
	{
		return SurfaceColour(maybeIntersection, lights, triangles, incidentRay);
	}
}

// void Fresnel(const vec3 &I, const vec3 &N, const float &ior, float &kr) 
// { 
//     float cosi = glm::dot(N, I); 
//     cosi = cosi > 1.0f ? 1.0f : cosi;
//   	cosi = cosi < -1.0f ? -1.0f : cosi;
//     float etai = 1, etat = ior; 
//     if (cosi > 0) { std::swap(etai, etat); } 
//     // Compute sini using Snell's law
//     float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi)); 
//     // Total internal reflection
//     if (sint >= 1) { 
//         kr = 1; 
//     } 
//     else { 
//         float cost = sqrtf(std::max(0.f, 1 - sint * sint)); 
//         cosi = fabsf(cosi); 
//         float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
//         float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)); 
//         kr = (Rs * Rs + Rp * Rp) / 2; 
//     } 
//     // As a consequence of the conservation of energy, transmittance is given by:
//     // kt = 1 - kr;
// }

float Fresnel(float c, float n)
{
	c = c > 1.0f ? 1.0f : c;
	c = c < -1.0f ? -1.0f : c;
	float R0 = ((n - 1)*(n - 1)) / ((n + 1)*(n + 1));
	float R = R0 + (1 - R0)*((float)pow(1 - c, 5));
	return R;
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

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights, const vector<Triangle> &triangles, vec3 incidentRay)
{
	return triangles[i.triangleIndex].color * (DirectLight(i, lights, triangles, incidentRay) + IndirectLight());
}

vec3 IndirectLight()
{
	return 0.2f*vec3(1, 1, 1);
}

// Compute the light intensity at an intersection resulting from direct light
vec3 DirectLight(const Intersection &intersection, vector<Light> &lights, const vector<Triangle> &triangles, vec3 incidentRay) {
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
			if (shadowRayIntersectionDistance <= lightDistance && (triangles[shadowRayIntersection.triangleIndex].transparency_ <= 0))
				continue; //this needs some revisiting
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

/** @brief Decides if ray is refracted or totally internally reflected,
 *         if refracted puts the direction of the refracted ray into t
 *
 *  @param d      The incident ray.
 *  @param normal The normal of the intersected surface
 *  @param n      The refractive index of the material the incident ray is coming from.
 *  @param nt     The refractive index of the material the ray is entering.
 *  @param t      The direction of the refracted ray.
 *  @return bool  Does ray refract.
 */
bool Refract(vec3 d, vec3 normal, float n, vec3 &t)
{
	float cosi = glm::dot(normal, d);
	float temp = 1 - (n*n*(1 - cosi* cosi));
	if (temp < 0) return false; //since no sqaureroot so total internl reflection occurs
	t = (n*(d - n*cosi)) - n * static_cast<float>(sqrt(temp));
	t = glm::normalize(t);
	return true;
}


// float cosi = -nhit.dot(raydir); 
// float temp = 1 - eta * eta * (1 - cosi * cosi); 
// Vec3f refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k)); 
// t = glm::normalize(t); 

vec3 Reflect(vec3 d, vec3 normal)
{
	return d - normal * 2.0f * glm::dot(d, normal);
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
