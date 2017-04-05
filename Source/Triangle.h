#pragma once

#include "Primitive.h"

class Triangle: public Primitive
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	// glm::vec3 color;
	// float reflectivity_;
	// float refractive_index_;

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float reflectivity, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal, float reflectivity, float refractive_index);

	void ComputeNormal();

	void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i);
};