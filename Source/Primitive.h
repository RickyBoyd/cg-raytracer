#pragma once
#include <glm/glm.hpp>

struct Intersection
{
	glm::vec3 position;
	float distance;
	int triangleIndex;
	glm::vec3 normal;
};

class Primitive
{
public:
	glm::vec3 color;
	float reflectivity_;
	float refractive_index_;

	virtual void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) = 0;

protected:
	Primitive(glm::vec3 color, float reflectivity_, float refractive_index_);
};