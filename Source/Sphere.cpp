#include "Sphere.h"

Sphere::Sphere(glm::vec3 centre, float radius, glm::vec3 colour, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(reflectivity, refractive_index), colour_(colour), centre_(centre), radius_(radius)
{
}

glm::vec3 Sphere::ComputeNormal(glm::vec3 point) const
{
	return glm::normalize(point - centre_);
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
// returns t value of intersection and intersection point q 

void Sphere::Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) 
{
	glm::vec3 m = start - centre_; 
	float b = glm::dot(m, dir); 
	float c = glm::dot(m, m) - radius_ * radius_; 

	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
	if (c > 0.0f && b > 0.0f) return; 
	float discr = b*b - c; 

	// A negative discriminant corresponds to ray missing sphere 
	if (discr < 0.0f) return; 

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float t = -b - glm::sqrt(discr); 

	// If t is negative, ray started inside sphere so clamp t to zero 
	if (t < 0.0f) t = 0.0f; 
	glm::vec3 q = start + t * dir; 
	if(t < intersection.distance)
	{
		intersection.position = q;
		intersection.distance = t;
		intersection.index = i;
		intersection.normal = ComputeNormal(q);
	}
}

glm::vec3 Sphere::GetAmbientColour(int u, int v)
{
	return colour_;
}

glm::vec3 Sphere::GetDiffuseColour(int u, int v)
{
	return colour_;
}