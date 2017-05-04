#include "Triangle.h"
#include "Material.h"
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(reflectivity, refractive_index), v0_(v0), v1_(v1), v2_(v2), material_(material)
{
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> material, glm::vec3 normal, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(reflectivity, refractive_index), v0_(v0), v1_(v1), v2_(v2), normal_(normal), material_(material)
{
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material, float reflectivity, float refractive_index)
	: Primitive(reflectivity, refractive_index), v0_(v0), v1_(v1), v2_(v2), vt0_(vt0), vt1_(vt1), vt2_(vt2), material_(material)
{
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2, std::shared_ptr<Material> material, glm::vec3 normal, float reflectivity, float refractive_index)
	: Primitive(reflectivity, refractive_index), v0_(v0), v1_(v1), v2_(v2), vt0_(vt0), vt1_(vt1), vt2_(vt2), normal_(normal), material_(material)
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
	// No intersection if we're hitting the back face
	if (refractive_index_ < 0.0001f && glm::dot(dir, normal_) >= 0.0) return;
	
	// Use Cramer's rule to calculate intersection p
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

	// If the intersection is nearer than the existing one, overwrite it
	if (t > 0 && t < intersection.distance) {
		intersection.position = v0_ + u * e1 + v * e2;
		intersection.distance = t;
		intersection.index = i;
		intersection.normal = normal_;
		intersection.u = u;
		intersection.v = v;
	}
}

glm::vec3 Triangle::GetAmbientColour(const Intersection& i)
{
	glm::vec2 uv = vt0_ + (vt1_ - vt0_) * i.u + (vt2_ - vt0_) * i.v;
	return material_->GetAmbientColour(uv.x, uv.y);
}

glm::vec3 Triangle::GetDiffuseColour(const Intersection& i)
{
	glm::vec2 uv = vt0_ + (vt1_ - vt0_) * i.u + (vt2_ - vt0_) * i.v;
	return material_->GetDiffuseColour(uv.x, uv.y);
}

glm::vec3 Triangle::GetSpecularColour(const Intersection& i)
{
	glm::vec2 uv = vt0_ + (vt1_ - vt0_) * i.u + (vt2_ - vt0_) * i.v;
	return material_->GetSpecularColour(uv.x, uv.y);
}

float Triangle::GetSpecularIntensity(const Intersection& i)
{
	glm::vec2 uv = vt0_ + (vt1_ - vt0_) * i.u + (vt2_ - vt0_) * i.v;
	return material_->GetSpecularIntensity(uv.x, uv.y);
}

float Triangle::GetSpecularExponent(const Intersection& i)
{
	return material_->specular_exponent_;
}
