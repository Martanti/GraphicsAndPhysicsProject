#pragma once
#include "GameObject.h"
class CInvisiblePlane : public CGameObject
{
public:
	CInvisiblePlane();
	CInvisiblePlane(vec3 vec3Position, vec3 vec3Normal);

	~CInvisiblePlane();

	void ProgramStart();
	void ProgramUpdate();
	void Draw();
};

