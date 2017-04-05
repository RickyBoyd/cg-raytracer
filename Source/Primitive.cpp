#include "Primitive.h"

Primitive::Primitive(glm::vec3 color, float reflectivity_, float refractive_index_)
	: color(color), reflectivity_(reflectivity_), refractive_index_(refractive_index_)
{
}