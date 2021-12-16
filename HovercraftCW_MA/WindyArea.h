#pragma once
#include "GameObject.h"
#include "RotattingBody.h"

class CWindyArea :public CGameObject
{
public:
	CWindyArea();
	~CWindyArea();

	void ProgramStart();
	void ProgramUpdate();
	CGameObject* Clone();
	void Draw();
private:

	static void BlowWind(CGameObject* pgoObject);
	static vec3 m_vec3WindDirection;
	static float m_fWindSpeed;

};