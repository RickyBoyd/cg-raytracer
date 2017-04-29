#pragma once
#include <glm/glm.hpp>

struct Intersection
{
	glm::vec3 position;
	float distance;
	int index;
	glm::vec3 normal;
};

class Primitive
{
public:
	float reflectivity_;
	float refractive_index_;

	virtual ~Primitive() = default;

	virtual glm::vec3 GetAmbientColour(int u, int v) = 0;
	virtual glm::vec3 GetDiffuseColour(int u, int v) = 0;
	virtual void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) = 0;

protected:
	Primitive(float reflectivity_, float refractive_index_);
};