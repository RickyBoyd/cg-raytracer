#include "Sphere.h"

Sphere::Sphere(glm::vec3 centre, float radius, glm::vec3 color, float reflectivity = 0.0f, float refractive_index = 1.0f)
	: Primitive(color, reflectivity, refractive_index), centre(centre), radius(radius)
{
}

glm::vec3 Sphere::ComputeNormal(glm::vec3 point)
{
	return glm::normalize(centre - point);
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
// returns t value of intersection and intersection point q 

void Sphere::Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i) 
{
	glm::vec3 m = start - centre; 
	float b = glm::dot(m, dir); 
	float c = glm::dot(m, m) - radius * radius; 

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

// void Sphere::Intersect(glm::vec3 start, glm::vec3 dir, Intersection &intersection, int i)
// {
// 	// Calculate ray start's offset from the sphere center
// 	float3 p = s - c;

// 	float rSquared = r * r;
// 	float p_d = dot(p, d);

// 	// The sphere is behind or surrounding the start point.
// 	if(p_d > 0 || dot(p, p) < rSquared) return;

// 	// Flatten p into the plane passing through c perpendicular to the ray.
// 	// This gives the closest approach of the ray to the center.
// 	float3 a = p - p_d * d;

// 	float aSquared = dot(a, a);

// 	// Closest approach is outside the sphere.
// 	if(aSquared > rSquared)
// 	return NO_COLLISION;

// 	// Calculate distance from plane where ray enters/exits the sphere.    
// 	float h = sqrt(rSquared - aSquared);

// 	// Calculate intersection point relative to sphere center.
// 	float3 i = a - h * d;

// 	float3 intersection = c + i;
// 	float3 normal = i/r;
// 	// We've taken a shortcut here to avoid a second square root.
// 	// Note numerical errors can make the normal have length slightly different from 1.
// 	// If you need higher precision, you may need to perform a conventional normalization.

// 	return (intersection, normal);
// }