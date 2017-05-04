#pragma once
#include <glm/glm.hpp>

struct Intersection
{
	glm::vec3 position;
	float distance;
	int index;
	glm::vec3 normal;
	float u, v; // Barycentric coordinates
};

class Primitive
{
public:
	float reflectivity_;
	float refractive_index_;

	virtual ~Primitive() = default;

	virtual glm::vec3 GetAmbientColour(const Intersection& i) = 0;
	virtual glm::vec3 GetDiffuseColour(const Intersection& i) = 0;
	virtual glm::vec3 GetSpecularColour(const Intersection& i) = 0;
	virtual float GetSpecularExponent(const Intersection& i) = 0;
	virtual float GetSpecularIntensity(const Intersection& i) = 0;
	virtual void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) = 0;

protected:
	Primitive(float reflectivity_, float refractive_index_);
};