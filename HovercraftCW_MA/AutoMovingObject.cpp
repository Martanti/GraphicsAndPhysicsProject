#include "AutoMovingObject.h"

CAutoMovingObject::CAutoMovingObject(string* rstrPathToModel, string* rstrPathToTexture, SMaterial* rmatrMaterial) : CGameObject(rstrPathToModel, rstrPathToTexture, rmatrMaterial)
{
	this->m_strName = "Moving object";

	this->m_tsTag.m_vstrHeldTags.emplace_back("Moving object");

}

CAutoMovingObject::~CAutoMovingObject()
{
}

void CAutoMovingObject::ProgramStart()
{
	this->m_vec3Destination1 = this->m_vec3Position;
	this->m_vec3Destination1.z -= 50;
	this->m_vec3CurrentDesitination = this->m_vec3Destination1;


	this->m_vec3Destination2 = this->m_vec3Position;
	this->m_vec3Destination2.z += 50;

	CRotattingBody* prbBody = new CRotattingBody();

	prbBody->m_bIsAffectedByGravity = false;

	this->m_pbdPhysicalBody = prbBody;
	m_fRemainingRotation = this->kfRotation;
}

void CAutoMovingObject::ProgramUpdate()
{
	CGameObject* pgoReadable = CTagSystem::GetTagged("Moving object")[0];
	CAutoMovingObject* pmoReadable = static_cast<CAutoMovingObject*>(pgoReadable);

	switch (pmoReadable->m_ecaCurrent)
	{
	case (CAutoMovingObject::ECurrentAction::Move):
		this->Move(pmoReadable);
		break;

	case (CAutoMovingObject::ECurrentAction::Rotate):
		this->Rotate(pmoReadable);
		break;
	}
}

CGameObject* CAutoMovingObject::Clone()
{
	CAutoMovingObject* pmovoClone = new CAutoMovingObject(*this);

	CRotattingBody* prbClone = new CRotattingBody(*(static_cast<CRotattingBody*>(pmovoClone->m_pbdPhysicalBody)));
	pmovoClone->m_pbdPhysicalBody = prbClone;
	pmovoClone->m_vec3CurrentDesitination = this->m_vec3CurrentDesitination;
	return pmovoClone;
}

void CAutoMovingObject::Rotate(CAutoMovingObject* pmoReadable)
{
	CRotattingBody* prbReadable = static_cast<CRotattingBody*>(pmoReadable->m_pbdPhysicalBody);
	CRotattingBody* prbWritable = static_cast<CRotattingBody*>(this->m_pbdPhysicalBody);

	if ( pmoReadable->m_fRemainingRotation <= 0.5f)
	{
		this->m_ecaCurrent = CAutoMovingObject::ECurrentAction::Move;
		pmoReadable->m_ecaCurrent = CAutoMovingObject::ECurrentAction::Move;

		this->m_fRemainingRotation = kfRotation;
		pmoReadable->m_fRemainingRotation = kfRotation;

		if (prbReadable->m_vec3Orientation.y >= 359)
		{
			prbWritable->m_vec3Orientation.y = 0;
			prbReadable->m_vec3Orientation.y = 0;
		}
		else
		{
			prbWritable->m_vec3Orientation.y = 180;
			prbReadable->m_vec3Orientation.y = 180;
		}
	}

	else
	{
		float fRotation = CGameObject::sm_fDeltaTime * m_fRotatingSpeed;

		prbWritable->m_vec3Orientation.y = prbReadable->m_vec3Orientation.y + fRotation;
		this->m_fRemainingRotation = pmoReadable->m_fRemainingRotation - fRotation;
	}
}

void CAutoMovingObject::Move(CAutoMovingObject* pmoReadable)
{
	
	float fDistance = glm::length(this->m_vec3Position - pmoReadable->m_vec3CurrentDesitination);


	if (fDistance <= this->m_MinDistance)
	{

		this->m_ecaCurrent = CAutoMovingObject::ECurrentAction::Rotate;
		pmoReadable->m_ecaCurrent = CAutoMovingObject::ECurrentAction::Rotate;


		if (glm::all(glm::equal(pmoReadable->m_vec3CurrentDesitination, this->m_vec3Destination2)))
		{
			this->m_vec3CurrentDesitination = this->m_vec3Destination1;
			pmoReadable->m_vec3CurrentDesitination = this->m_vec3Destination1;
		}
		else
		{
			this->m_vec3CurrentDesitination = this->m_vec3Destination2;
			pmoReadable->m_vec3CurrentDesitination = this->m_vec3Destination2;
		}
	}

	else
	{
		vec3 vec3Movement = glm::normalize((pmoReadable->m_vec3CurrentDesitination - pmoReadable->m_vec3Position)) * this->m_fSpeed;
		this->m_vec3Position = pmoReadable->m_vec3Position + vec3Movement * CGameObject::sm_fDeltaTime;
	}
}
