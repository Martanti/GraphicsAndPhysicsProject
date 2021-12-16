#include "GameObject.h"
#include "Input.h"
#include "RotattingBody.h"
#include <GL/freeglut.h>
#include "vertex.h"
#include "Collider.h"

#pragma once
class CTestSphere : public CGameObject
{
public:
	CTestSphere(SMaterial* pmatrMaterial);
	~CTestSphere();

	void ProgramStart();
	void ProgramUpdate();
};