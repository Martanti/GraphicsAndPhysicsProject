#pragma region Includes
#include "GameObject.h"
#include "Input.h"
#include "RotattingBody.h"
#include <GL/freeglut.h>
#include "vertex.h"
#include "getbmp.h"
#pragma endregion

#pragma once

class CPlayer : public CGameObject
{
public:
	CPlayer(SMaterial* pmatrMaterial);
	~CPlayer();

	void ProgramStart();
	void ProgramUpdate();
	void Draw();
	CGameObject* Clone();
};