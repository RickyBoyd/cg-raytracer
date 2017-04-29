#pragma once

#include "Primitive.h"
#include <algorithm>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>

class Sphere: public Primitive
{
public:
	glm::vec3 colour_;
	glm::vec3 centre_;
	float radius_;

	Sphere(glm::vec3 centre, float radius, glm::vec3 color, float reflectivity, float refractive_index);

	glm::vec3 ComputeNormal(glm::vec3 point) const;

	void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) override;
	glm::vec3 GetAmbientColour(const Intersection& i) override;
	glm::vec3 GetDiffuseColour(const Intersection& i) override;
};
