#include "WindyArea.h"

float CWindyArea::m_fWindSpeed = 3.5f;
vec3 CWindyArea::m_vec3WindDirection = glm::normalize(vec3(1, 0, 1));

CWindyArea::CWindyArea()
{
	this->m_strName = "Windy area";

	auto pscolCollider = std::make_shared<CSphereCollider>(&(this->m_vec3Position));
	pscolCollider->m_fRadius = 8;
	this->m_vec3Scale = {8, 8, 8};
	
	pscolCollider->m_bIsTrigger = true;
	pscolCollider->OnTriggerEnter = BlowWind;

	this->m_pcolCollider = pscolCollider;

	this->m_tsTag.m_vstrHeldTags.emplace_back("Collides");

	this->m_bIsStatic = true;

}

CWindyArea::~CWindyArea()
{
}

void CWindyArea::ProgramStart()
{
}

void CWindyArea::ProgramUpdate()
{

}

CGameObject* CWindyArea::Clone()
{
	CWindyArea* pwaClone = new CWindyArea(*this);
	return pwaClone;
}

void CWindyArea::Draw()
{
	if (*CGameObject::sm_pbDebugMode) {
		glUniform1ui(glGetUniformLocation(*CGameObject::sm_puiProgramID, "uiVisualizationType"), (int)CGameObject::EVisualizationType::NO_DATA);
		this->m_pcolCollider->DrawShape();
	}
}

void CWindyArea::BlowWind(CGameObject* pgoObject)
{
	vec3 vec3Wind = CWindyArea::m_vec3WindDirection * CWindyArea::m_fWindSpeed;
	vec3 vec3WindPosition = { 1,1,1 };

	CRotattingBody* prbCurrent = static_cast<CRotattingBody*>(pgoObject->m_pbdPhysicalBody);

	if (prbCurrent!= nullptr)
	{
		prbCurrent->AddForce(vec3Wind, vec3WindPosition, CRotattingBody::EForceType::Enviromental);
	}
	else if(pgoObject->m_pbdPhysicalBody != nullptr)
	{
		pgoObject->m_pbdPhysicalBody->AddForce(vec3Wind, vec3WindPosition);
	}
}
