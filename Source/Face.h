#pragma once
#include "Material.h"
#include "TestModel.h"
#include <experimental/optional>

class Face
{
public:
	std::vector<glm::vec3> vertices_;
	std::experimental::optional<std::vector<glm::vec2>> texture_coords_;
	std::experimental::optional<glm::vec3> normal_;
	Material material_;

	Face(std::vector<glm::vec3> vertices, std::experimental::optional<std::vector<glm::vec2>> texture_coords, std::experimental::optional<glm::vec3> normal, Material material);
	Face(const Face& face);
	~Face();

	Triangle ToTriangle(glm::vec3 transform);
};

