#pragma once

#include "Primitive.h"
#include <algorithm>

class Sphere: public Primitive
{
public:
	glm::vec3 colour_;
	glm::vec3 centre_;
	float radius_;

	Sphere(glm::vec3 centre, float radius, glm::vec3 color, float reflectivity, float refractive_index);

	glm::vec3 ComputeNormal(glm::vec3 point) const;

	void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) override;
	glm::vec3 GetAmbientColour(int u, int v) override;
	glm::vec3 GetDiffuseColour(int u, int v) override;
};