#pragma once
#include "Material.h"
#include "TestModel.h"
#include <optional>

class Face
{
public:
	std::vector<glm::vec3> vertices_;
	std::optional<std::vector<glm::vec2>> texture_coords_;
	std::optional<glm::vec3> normal_;
	Material material_;

	Face(std::vector<glm::vec3> vertices, std::optional<std::vector<glm::vec2>> texture_coords, std::optional<glm::vec3> normal, Material material);
	Face(const Face& face);
	~Face();

	Triangle ToTriangle(glm::vec3 transform);
};

