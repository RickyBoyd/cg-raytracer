#pragma once
#include "Material.h"
#include "Triangle.h"
#include "Sphere.h"

class Face
{
public:
	std::vector<glm::vec3> vertices_;
	std::shared_ptr<std::vector<glm::vec2>> texture_coords_;
	std::shared_ptr<glm::vec3> normal_;
	Material material_;
	bool isSphere_;
	float radius_;

	Face(std::vector<glm::vec3> vertices, std::shared_ptr<std::vector<glm::vec2>> texture_coords, std::shared_ptr<glm::vec3> normal, Material material);
	Face(const Face& face);
	Face(std::vector<glm::vec3> vertices, float radius, Material material);
	~Face();

	Primitive* ToPrimitive(glm::vec3 transform, glm::vec3 scale);
};

