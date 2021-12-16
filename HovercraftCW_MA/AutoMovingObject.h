#pragma once
#include "GameObject.h"
#include "Time.h"
#include "RotattingBody.h"

class CAutoMovingObject :public CGameObject
{
public:

	CAutoMovingObject(string* rstrPathToModel, string* rstrPathToTexture, SMaterial* rmatrMaterial);
	~CAutoMovingObject();

	void ProgramStart();
	void ProgramUpdate();
	CGameObject* Clone();

	enum class ECurrentAction { Move, Rotate };

	CAutoMovingObject::ECurrentAction m_ecaCurrent = CAutoMovingObject::ECurrentAction::Move;
	float m_fRemainingRotation;

private:

	float m_fSpeed = 8.f;

	vec3 m_vec3Destination1;
	vec3 m_vec3Destination2;

	vec3 m_vec3CurrentDesitination;

	float m_MinDistance = 0.1f;

	const float kfRotation = 180;

	float m_fRotatingSpeed = 45.f;

	void Rotate(CAutoMovingObject* pmoReadable);
	void Move(CAutoMovingObject* pmoReadable);
};

