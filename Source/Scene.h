#pragma once
#include "Model.h"
#include "Light.h"
#include "Camera.h"

struct ModelInstance
{
	ModelInstance(const Model& model, const glm::vec3& transform)
		: model(model),
		  transform(transform)
	{
	}

	Model model;
	glm::vec3 transform;
};

class Scene
{
public:
	std::vector<ModelInstance> model_instances_;
	std::vector<Light> lights_;
	Camera camera_;

	Scene(std::vector<ModelInstance> model_instances, std::vector<Light> lights_, Camera camera);
	~Scene();

	std::vector<Triangle> ToTriangles() const;
};

