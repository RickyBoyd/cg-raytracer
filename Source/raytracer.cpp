#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "Triangle.h"
#include "Primitive.h"
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
#define RAY_DEPTH 8

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

void Draw(Scene &scene, const vector<Primitive*> &primitives);

void Interpolate(float a, float b, vector<float> &result);

bool ClosestIntersection(vec3 start, vec3 dir, const vector<Primitive*> &primitives, Intersection &closestIntersection);

vec3 Trace(vec3 startPos, vec3 incidentRay, vector<Light> lights, const vector<Primitive*> &primitives, int depth);

float Fresnel(float c, float n);
//void Fresnel(const vec3 &I, const vec3 &N, const float &ior, float &kr) ;
vec3 DirectLight(const Intersection &i, vector<Light> &lights, const vector<Primitive*> &primitives, vec3 incidentRay);

vec3 IndirectLight();

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights, const vector<Primitive*> &primitives, vec3 incidentRay);

void AddLight(vec3 pos, vec3 color, vector<Light> &lights);

bool Refract(vec3 d, vec3 normal, float n, vec3 &t);

vec3 Reflect(vec3 d, vec3 normal);

void DisplaySampleTexture();

int main(int argc, char *argv[]) {

	if (argc > 1 && strncmp(argv[1], "texture", 7) == 0)
	{
		DisplaySampleTexture();
		return 0;
	}

	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();    // Set start value for timer.

	Uint8 lightSelected = 0;

	auto cubeScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cube.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto cornellBoxScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/cornell_box.obj")), 
			ModelInstance(Model("Resources/blocks.obj")) 
		},
		std::vector<Light> { Light { vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera { glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto cornellBoxTransparentScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/cornell_box.obj")), 
			ModelInstance(Model("Resources/blocks_transparent.obj")) 
		},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto cornellBoxSphereScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/cornell_box.obj")), 
			ModelInstance(Model("Resources/sphere.obj"), glm::vec3(0, 0.5, 0)),
			ModelInstance(Model("Resources/sphere_glass.obj"), glm::vec3(0, -0.5, 0))
		},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto cornellBoxTexturedScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/cornell_box.obj")), 
			ModelInstance(Model("Resources/cube_textured.obj"), glm::vec3(-0.5f, -0.5f, -0.5f))
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


	auto bunnyBoxScene = Scene(
		std::vector<ModelInstance> { 
			ModelInstance(Model("Resources/cornell_box_empty.obj")),
			ModelInstance(Model("Resources/bunny_transparent.obj"), glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(12.0f, 12.0f, 12.0f)) 
		},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });
#endif

	Scene &scene = cornellBoxTexturedScene;

	std::vector<Primitive*> scenePrimitives = scene.ToPrimitives();
	cout << "Loaded " << scenePrimitives.size() << " primtives" << endl;

	while (NoQuitMessageSDL()) {
		Draw(scene, scenePrimitives);
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

void Draw(Scene &scene, const vector<Primitive*> &primitives) {
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
				vec3 triangleColour = Trace(scene.camera_.position, d, scene.lights_, primitives, 0);
				colour += triangleColour;
				//sampleTriangleIndices[sample] = maybeIntersection.triangleIndex;
				sampleTriangleColours[sample] = triangleColour;
				//}
#else
				colour += Trace(cameraPos, d, lights, 0);
#endif

			}
			PutPixelSDL(screen, x, y, colour / float(AA_SAMPLES));
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

vec3 Trace(vec3 startPos, vec3 incidentRay, vector<Light> lights, const vector<Primitive*> &primitives, int depth)
{
	// Find an intersection of this ray with the model, if exists
	float bias = 1e-3;
	Intersection maybeIntersection;
	incidentRay = glm::normalize(incidentRay);
	bool hasIntersection = ClosestIntersection(startPos, incidentRay, primitives, maybeIntersection);
	if (!hasIntersection)
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
	Primitive* primitive = primitives[maybeIntersection.index];
	if (primitive->refractive_index_ > 0.001f && depth <= RAY_DEPTH)
	{
		vec3 normal = maybeIntersection.normal;
		float c;
		float refractiveIndex = primitive->refractive_index_;

		vec3 refractionRay(0.0f, 0.0f, 0.0f);
		normal = glm::normalize(normal);
		incidentRay = glm::normalize(incidentRay);

		vec3 reflectionRay;
		vec3 reflectionColor;

		vec3 zero(0.0f, 0.0f, 0.0f);
		bool inside = glm::dot(incidentRay, normal) < 0;
		if (glm::dot(incidentRay, normal) < 0)
		{
			refractionRay = glm::refract(incidentRay, normal, refractiveIndex);

			reflectionRay = glm::reflect(incidentRay, normal);
			reflectionColor = Trace(maybeIntersection.position + normal*bias, reflectionRay, lights, primitives, depth + 1);

			c = -1.0f*glm::dot(incidentRay, normal);
			normal = -1.0f*normal;
		}
		else
		{
			//bool refractionOccurs = Refract( incidentRay, -1.0f*normal, 1.0f/refractiveIndex, refractionRay);

			reflectionRay = glm::reflect(incidentRay, -1.0f*normal);
			reflectionColor = Trace(maybeIntersection.position -1.0f*normal*bias, reflectionRay, lights, primitives, depth + 1);
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
		float R = Fresnel(c, refractiveIndex);
		vec3 refractionColor = Trace(maybeIntersection.position + normal*bias, refractionRay, lights, primitives, depth + 1);
		return (1.0f - R)*refractionColor + R*reflectionColor;
	} else if (primitive->reflectivity_ > 0.001f && depth <= RAY_DEPTH)
	{
		vec3 normal = maybeIntersection.normal;

		vec3 refractionRay(0.0f, 0.0f, 0.0f);
		normal = glm::normalize(normal);
		incidentRay = glm::normalize(incidentRay);

		vec3 reflectionRay = glm::reflect(incidentRay, normal);
		vec3 reflectionColor = Trace(maybeIntersection.position, reflectionRay, lights, primitives, depth + 1);
		return reflectionColor;
	}
	else
	{
		return SurfaceColour(maybeIntersection, lights, primitives, incidentRay);
	}
}

float Fresnel(float c, float n)
{
	c = c > 1.0f ? 1.0f : c;
	c = c < -1.0f ? -1.0f : c;
	float R0 = ((n - 1)*(n - 1)) / ((n + 1)*(n + 1));
	float R = R0 + (1 - R0)*((float)pow(1 - c, 5));
	return R;
}

/// <summary>Find the triangle intersected first by the ray from start in direction dir.</summary>
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Primitive*> &primitives, Intersection &closestIntersection) {
	// Initialise the closest intersection to infinitely far away
	closestIntersection.distance = std::numeric_limits<float>::max();

	// Check each triangle for an intersection using the Moller-Trumbore algorithm
	// TODO: optimise this process using bounding boxes
	for (int i = 0; i < primitives.size(); i++) {
		primitives[i]->Intersect(start, dir, closestIntersection, i);
	}
	return closestIntersection.distance != std::numeric_limits<float>::max();
}

/// <summary>Find the triangle intersected first by the ray from start in direction dir.</summary>
// bool ClosestIntersection(vec3 start, vec3 dir, const vector<Primitive*> &primitives, int currentTriangleIndex, Intersection &closestIntersection) {
// 	// Initialise the closest intersection to infinitely far away
// 	closestIntersection.distance = std::numeric_limits<float>::max();

// 	// Check each triangle for an intersection using the Moller-Trumbore algorithm
// 	// TODO: optimise this process using bounding boxes
// 	for (int i = 0; i < primitives.size(); i++) {
// 		if (i == currentTriangleIndex) continue;



// 		// else
// 		// {
// 		// 	glm::vec3 c = sphere.centre_;
// 		// 	float radius = sphere.radius_l
// 		// 	glm::vec3 t1 = start - c;
// 		// 	glm::vec3 t2 = d * t1;
// 		// 	glm::vec3 t3 = t2*t2 - t1*t1 - radius*radius;
// 		// 	if(t3 < 0.0f) //no real solution so no intersection
// 		// 	{
// 		// 		continue;
// 		// 	}
// 		// 	else
// 		// 	{
// 		// 		float d1 = -t2 - glm::sqrt(t3);
// 		// 		float d2 = -t2 + glm::sqrt(t3);
// 		// 		float distance = std::min(d1, d2);
// 		// 		closestIntersection.position = start + distance * dir;
// 		// 		closestIntersection.distance = distance;
// 		// 		closestIntersection.triangleIndex = i;
// 		// 	}
// 		// }
// 	}
// 	return closestIntersection.distance != std::numeric_limits<float>::max();
// }

vec3 SurfaceColour(const Intersection &i, vector<Light> &lights, const vector<Primitive*> &primitives, vec3 incidentRay)
{
	return primitives[i.index]->GetDiffuseColour(i) * DirectLight(i, lights, primitives, incidentRay) +
		primitives[i.index]->GetAmbientColour(i) * IndirectLight();
}

vec3 IndirectLight()
{
	return 0.2f*vec3(1, 1, 1);
}

// Compute the light intensity at an intersection resulting from direct light
vec3 DirectLight(const Intersection &intersection, vector<Light> &lights, const vector<Primitive*> &primitives, vec3 incidentRay) {
	float Kdiffuse = 0.8f;
	float Kspecular = 0.5f;
	float bias = 1e-3;
	glm::vec3 lightIntensity(0.0f, 0.0f, 0.0f);
	glm::vec3 specularIntensity(0.0f, 0.0f, 0.0f);
	for (auto light : lights) {

		// Determine whether the shadow ray (intersection->light) intersects with another triangle
		vec3 shadowRay = light.position - intersection.position;
		float lightDistance = glm::length(shadowRay);
		Intersection shadowRayIntersection;
		shadowRay = glm::normalize(shadowRay);
		bool hasIntersection = ClosestIntersection(intersection.position + intersection.normal * bias, shadowRay, primitives, shadowRayIntersection);

		// If an intersection was found, and it is between the intersection and the light, this light does not reach the intersection
		if (hasIntersection) {
			float shadowRayIntersectionDistance = glm::length(intersection.position - shadowRayIntersection.position);
			if (shadowRayIntersectionDistance <= lightDistance && (primitives[shadowRayIntersection.index]->refractive_index_ <= 0))
				continue; //this needs some revisiting
		}

		// Otherwise, compute the light's power per unit area at the intersection
		vec3 B = (light.color / static_cast<float>(4 * lightDistance * lightDistance * M_PI));
		// Compute a normalising factor based on the angle between the shadow ray and the surface normal
		float dp = abs(glm::dot(glm::normalize(intersection.normal), glm::normalize(shadowRay)));
		lightIntensity += B * glm::max(dp, 0.0f);

		// Compute direction of 'ideal' reflection from light source
		vec3 idealReflection = glm::normalize(glm::reflect(shadowRay, shadowRayIntersection.normal));
		// Project actual incident ray onto reflection and use Phong to calculate specular intensity
		specularIntensity += B * static_cast<float>(std::pow(std::max(0.0f, glm::dot(idealReflection, glm::normalize(incidentRay))), 100));

	}
	return lightIntensity * Kdiffuse + specularIntensity * Kspecular;
}

bool Refract(vec3 d, vec3 normal, float n, vec3 &t)
{
	float cosi = glm::dot(normal, d);
	float temp = 1 - (n*n*(1 - cosi* cosi));
	if (temp < 0) return false; //since no sqaureroot so total internl reflection occurs
	t = (n*(d - n*cosi)) - n * static_cast<float>(sqrt(temp));
	t = glm::normalize(t);
	return true;
}

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

void DisplaySampleTexture()
{
	auto materials = Material::LoadMaterials("Resources/textured.mtl");
	vector<std::shared_ptr<Material>>::iterator cube_material = std::find_if(materials.begin(), materials.end(), [](shared_ptr<Material> m) { return m->name_ == "cube"; });
	if (cube_material != materials.end())
	{
		shared_ptr<Material> m = *cube_material;
		screen = InitializeSDL(m->ambient_texture_x_, m->ambient_texture_y_);

		while (NoQuitMessageSDL()) {
			if (SDL_MUSTLOCK(screen))
				SDL_LockSurface(screen);

			for (int y = 0; y < m->ambient_texture_y_; y++) {
				for (int x = 0; x < m->ambient_texture_x_; x++) {
					PutPixelSDL(screen, x, y, glm::vec3(
						float(m->ambient_texture_[(y * m->ambient_texture_x_ * m->ambient_texture_n_) + (x * m->ambient_texture_n_)]) / 255.0f,
						float(m->ambient_texture_[(y * m->ambient_texture_x_ * m->ambient_texture_n_) + (x * m->ambient_texture_n_) + 1]) / 255.0f,
						float(m->ambient_texture_[(y * m->ambient_texture_x_ * m->ambient_texture_n_) + (x * m->ambient_texture_n_) + 2]) / 255.0f));
				}
			}

			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);

			SDL_UpdateRect(screen, 0, 0, 0, 0);
		}
	}
}