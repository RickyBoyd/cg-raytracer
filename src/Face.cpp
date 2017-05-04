#include "Face.h"



Face::Face(std::vector<glm::vec3> vertices, std::shared_ptr<std::vector<glm::vec2>> texture_coords, std::shared_ptr<glm::vec3> normal, std::shared_ptr<Material> material)
	: vertices_(vertices), texture_coords_(texture_coords), normal_(normal), material_(material)
{
	isSphere_ = false;
}

Face::Face(const Face& face) 
	: vertices_(face.vertices_), texture_coords_(face.texture_coords_), normal_(face.normal_), material_(face.material_), isSphere_(face.isSphere_), radius_(face.radius_)
{
}

Face::Face(std::vector<glm::vec3> vertices, float radius, std::shared_ptr<Material> material)
	: vertices_(vertices), material_(material), radius_(radius)
{
	isSphere_ = true;
}

Face::~Face()
{
}

Primitive* Face::ToPrimitive(glm::vec3 transform, glm::vec3 scale)
{
	Primitive *primitive;
	if(isSphere_)
	{
		primitive = new Sphere(
			vertices_[0] + transform,
			radius_,
			material_->ambient_colour_,
			material_->reflectivity_,
			material_->refractive_index_);
	}
	else if (normal_ != nullptr) {
		if (texture_coords_->size() == 3)
		{
			primitive = new Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				(*texture_coords_)[0],
				(*texture_coords_)[1],
				(*texture_coords_)[2],
				material_,
				*normal_,
				material_->reflectivity_,
				material_->refractive_index_);
		} else
		{
			primitive = new Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				material_,
				*normal_,
				material_->reflectivity_,
				material_->refractive_index_);
		}
	}
	else
	{
		if (texture_coords_->size() == 3)
		{
			primitive = new Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				(*texture_coords_)[0],
				(*texture_coords_)[1],
				(*texture_coords_)[2],
				material_,
				material_->reflectivity_,
				material_->refractive_index_);
		} else
		{
			primitive = new Triangle(
				vertices_[0] * scale + transform,
				vertices_[1] * scale + transform,
				vertices_[2] * scale + transform,
				material_,
				material_->reflectivity_,
				material_->refractive_index_);
		}
	}
	return primitive;
}
