#include "GameObject.h"
#include "Time.h"
#include <GL\glew.h>

#pragma once

using glm::vec4;

class CLight : public CGameObject
{
public:

	enum class ELightType{
		DIRECTIONAL,
		POINT_OF_LIGHT
	};
	
	vec3 m_vec3AmbientColour;
	vec3 m_vec3DiffuseColour;
	vec3 m_vec3SpecularColour;
	vec3 m_vec3Direction;

	static vec3 sm_vec3GlobalAmbience;

	ELightType m_ltType;

	void ProgramStart();

	void ProgramUpdate();

	CGameObject* Clone();

	void Draw();
};

