#include "Face.h"



Face::Face(std::vector<glm::vec3> vertices, std::optional<std::vector<glm::vec2>> texture_coords, std::optional<glm::vec3> normal, Material material)
	: vertices_(vertices), texture_coords_(texture_coords), normal_(normal), material_(material)
{
}

Face::Face(const Face& face) 
	: vertices_(face.vertices_), texture_coords_(face.texture_coords_), normal_(face.normal_), material_(face.material_)
{
}


Face::~Face()
{
}

Triangle Face::ToTriangle(glm::vec3 transform)
{
	if (normal_.has_value()) {
		return Triangle(
			vertices_[0] + transform,
			vertices_[1] + transform,
			vertices_[2] + transform,
			material_.ambient_colour_,
			normal_.value());
	}
	else
	{
		return Triangle(
			vertices_[0] + transform,
			vertices_[0] + transform,
			vertices_[0] + transform,
			material_.ambient_colour_);
	}
}
