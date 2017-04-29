#pragma once

#include "Primitive.h"
#include <memory>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>

class Material;

class Triangle: public Primitive
{
public:
	glm::vec3 v0_;
	glm::vec3 v1_;
	glm::vec3 v2_;
	glm::vec2 vt0_;
	glm::vec2 vt1_;
	glm::vec2 vt2_;
	glm::vec3 normal_;
	std::shared_ptr<Material> material_;

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, float reflectivity, float refractive_index);
	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, glm::vec3 normal, float reflectivity, float refractive_index);

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material, float reflectivity, float refractive_index);
	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material, glm::vec3 normal, float reflectivity, float refractive_index);

	void ComputeNormal();

	void Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) override;
	glm::vec3 GetAmbientColour(const Intersection& i) override;
	glm::vec3 GetDiffuseColour(const Intersection& i) override;
};
