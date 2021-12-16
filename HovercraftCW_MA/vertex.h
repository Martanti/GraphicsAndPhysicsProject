#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
using glm::vec4;
using glm::vec2;
using glm::vec3;

struct Vertex
{
   vec4 coords;
   vec2 texCoords;
};

struct VertexWtihNormal
{
	vec4 coords;
	vec3 normals;
};

#endif
