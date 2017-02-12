#pragma once
#include <glm/detail/type_vec3.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <regex>
#include "TestModel.h"

class Model
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> face_vertex_indices;
	std::vector<glm::vec3> face_texture_indices;
	std::vector<glm::vec3> face_normal_indices;

	Model(std::vector<glm::vec3> vertices, std::vector<glm::vec3> face_vertex_indices);

	Model(std::string filename);

	///<summary>Return a vector of the substrings when str is split at delimiters matching regex</summary>
	static std::vector<std::string> SplitString(const std::string& str, const std::string& regex);

	std::vector<Triangle> ToTriangles() const;
};
