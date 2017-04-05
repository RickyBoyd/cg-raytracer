#include "Triangle.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(color, reflectivity, refractive_index), v0(v0), v1(v1), v2(v2)
{
	ComputeNormal();
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(color, reflectivity, refractive_index), v0(v0), v1(v1), v2(v2), normal(normal)
{
}

void Triangle::ComputeNormal()
{
	glm::vec3 e1 = v1 - v0;
	glm::vec3 e2 = v2 - v0;
	normal = glm::normalize(glm::cross(e2, e1));
}

void Triangle::Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i)
{
	glm::vec3 e1 = v1 - v0;
	glm::vec3 e2 = v2 - v0;
	glm::vec3 pvec = glm::cross(dir, e2);
	float det = glm::dot(e1, pvec);

	if (fabs(det) < 0.000000000001) //no intersection
	{
		return;	
	}

	float invDet = 1.0f / det;

	glm::vec3 tvec = start - v0;
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
		intersection.position = v0 + u * e1 + v * e2;
		intersection.distance = t;
		intersection.index = i;
		intersection.normal = normal;
	}
}