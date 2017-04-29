#include "Triangle.h"
#include "Material.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(reflectivity, refractive_index), v0_(v0), v1_(v1), v2_(v2), material_(material)
{
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, glm::vec3 normal, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(reflectivity, refractive_index), v0_(v0), v1_(v1), v2_(v2), normal_(normal), material_(material)
{
}

void Triangle::ComputeNormal()
{
	glm::vec3 e1 = v1_ - v0_;
	glm::vec3 e2 = v2_ - v0_;
	normal_ = glm::normalize(glm::cross(e2, e1));
}

void Triangle::Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i)
{
	glm::vec3 e1 = v1_ - v0_;
	glm::vec3 e2 = v2_ - v0_;
	glm::vec3 pvec = glm::cross(dir, e2);
	float det = glm::dot(e1, pvec);

	if (fabs(det) < 0.000000000001) //no intersection
	{
		return;	
	}

	float invDet = 1.0f / det;

	glm::vec3 tvec = start - v0_;
	float u = glm::dot(tvec, pvec) * invDet;
	if (u < 0)  //no intersection
	{
		return;
	}
	glm::vec3 qvec = glm::cross(tvec, e1);
	float v = glm::dot(dir, qvec) * invDet;
	if (v < 0 || u + v > 1) //no intersection
	{
		return; 
	}

	float t = glm::dot(e2, qvec) * invDet;

	if (t > 0 && t < intersection.distance) {
		intersection.position = v0_ + u * e1 + v * e2;
		intersection.distance = t;
		intersection.index = i;
		intersection.normal = normal_;
	}
}

glm::vec3 Triangle::GetAmbientColour(int u, int v)
{
	return material_->GetAmbientColour(u, v);
}

glm::vec3 Triangle::GetDiffuseColour(int u, int v)
{
	return material_->GetDiffuseColour(u, v);
}
