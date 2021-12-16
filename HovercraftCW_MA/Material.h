#include "glm/vec4.hpp"

#pragma once

using glm::vec3;

struct  SMaterial
{
	vec3 m_vec3Ambient;
	vec3 m_vec3Diffuse;
	vec3 m_vec3Specular;
	vec3 m_vec3Emission;
	float m_fGloss = 0;
};
