#include "Scene.h"



Scene::Scene(std::vector<ModelInstance> model_instances, std::vector<Light> lights, Camera camera) 
	: model_instances_(model_instances), lights_(lights), camera_(camera)
{
}


Scene::~Scene()
{
}

std::vector<Primitive*> Scene::ToPrimitives() const
{
	std::vector<Primitive*> scene_prims;
	for (auto model_instance : model_instances_)
	{
		auto model_prims = model_instance.model.ToPrimitives(model_instance.transform, model_instance.scale_);
		scene_prims.insert(scene_prims.end(), model_prims.begin(), model_prims.end());
	}

	return scene_prims;
}
