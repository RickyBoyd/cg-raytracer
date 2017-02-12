#include "Model.h"

Model::Model(std::vector<glm::vec3> vertices, std::vector<glm::vec3> face_vertex_indices) : vertices(vertices), face_vertex_indices(face_vertex_indices) {}

Model::Model(std::string filename)
{
	std::ifstream is(filename);

	for (std::string str; std::getline(is, str);)
	{
		std::vector<std::string> tokens = SplitString(str, "\\s+");

		if (tokens[0].compare("v") == 0)
		{
			auto vertex = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			vertices.push_back(vertex);
		}
		else if (tokens[0].compare("f") == 0)
		{
			std::vector<std::string> vertex1 = SplitString(tokens[1], "/");
			std::vector<std::string> vertex2 = SplitString(tokens[2], "/");
			std::vector<std::string> vertex3 = SplitString(tokens[3], "/");
			if (vertex1.size() >= 1 && vertex2.size() >= 1 && vertex3.size() >= 1)
			{
				try
				{
					face_vertex_indices.push_back(glm::vec3(std::stoi(vertex1[0]), std::stoi(vertex2[0]), std::stoi(vertex3[0])));
				}
				catch (std::exception e) {}
			}
			if (vertex1.size() >= 2 && vertex2.size() >= 2 && vertex3.size() >= 2)
			{
				try
				{
					face_texture_indices.push_back(glm::vec3(std::stoi(vertex1[1]), std::stoi(vertex2[1]), std::stoi(vertex3[1])));
				}
				catch (std::exception e) {}
			}
			if (vertex1.size() >= 3 && vertex2.size() >= 3 && vertex3.size() >= 3)
			{
				try
				{
					face_normal_indices.push_back(glm::vec3(std::stoi(vertex1[2]), std::stoi(vertex2[2]), std::stoi(vertex3[2])));
				}
				catch (std::exception e) {}
			}
		}
		else if (tokens[0].compare("vn") == 0)
		{
			auto normal = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
			normals.push_back(normal);
		}
	}
}

std::vector<std::string> Model::SplitString(const std::string& str, const std::string& regex)
{
	std::regex re(regex);
	std::sregex_token_iterator first( str.begin(), str.end(), re, -1 );
	std::sregex_token_iterator last;
	return std::vector<std::string>(first, last);
}

std::vector<Triangle> Model::ToTriangles(const glm::vec3 transform) const
{
	std::vector<Triangle> tris;
	tris.reserve(face_vertex_indices.size());
	for (int i = 0; i < face_vertex_indices.size(); i++)
	{
		if (face_normal_indices.size() > 0) {
			Triangle tri = Triangle(
				vertices[face_vertex_indices[i].x - 1] + transform,
				vertices[face_vertex_indices[i].y - 1] + transform,
				vertices[face_vertex_indices[i].z - 1] + transform,
				glm::vec3(0.75f, 0.15f, 0.15f),
				normals[face_normal_indices[i].x - 1]);
			tris.push_back(tri);
		} else
		{
			Triangle tri = Triangle(
				vertices[face_vertex_indices[i].x - 1] + transform,
				vertices[face_vertex_indices[i].y - 1] + transform,
				vertices[face_vertex_indices[i].z - 1] + transform,
				glm::vec3(0.75f, 0.15f, 0.15f));
			tris.push_back(tri);
		}
	}
	return tris;
}