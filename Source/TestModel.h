#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;
	float transparency;
	float reflectivity;

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float transparency, float reflectivity )
		: v0(v0), v1(v1), v2(v2), color(color), transparency(transparency), reflectivity(reflectivity)
	{
		ComputeNormal();
	}

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, float transparency, float reflectivity )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		reflectivity = 0;
		transparency = 0;
		ComputeNormal();
	}

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color);
	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 normal);

	void ComputeNormal();
};
void LoadTestModel(std::vector<Triangle>& triangles);

#endif