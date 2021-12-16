#include "Player.h"

#include <algorithm>
#include <glm\gtx\string_cast.hpp>

CPlayer::CPlayer(SMaterial * pmatrMaterial) : CGameObject(&(string) "../Geometry/Hovercraft.obj", & (string)"../Geometry/HovercraftTexture.jpg", pmatrMaterial)
{
	this->m_strName = "Player";

	this->m_tsTag.m_vstrHeldTags.emplace_back("Player");
	this->m_tsTag.m_vstrHeldTags.emplace_back("Collides");

#pragma region Movement
	CRotattingBody* prbCurrent = new CRotattingBody();
	
	prbCurrent->m_bIsAffectedByGravity = false;

	float fMass = 3;
	prbCurrent->m_fMass = fMass;
	prbCurrent->m_fInvertedMass = 1 / fMass;	
	prbCurrent->m_fVelocityDamp = 0.8f;

	prbCurrent->m_mat3Inertia = mat3(
		fMass * 0.0833333333333333f  * 9, 0, 0,
		0 , fMass * 0.0833333333333333f * 9, 0,
		0 , 0, fMass * 0.0833333333333333f * 9);


	prbCurrent->m_mat3InvertedInertia = glm::inverse(prbCurrent->m_mat3Inertia);
	
	prbCurrent->m_fAngularDamp = 0.55f;

	this->m_pbdPhysicalBody = prbCurrent;
#pragma endregion

#pragma region Collider
	//https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=vs-2019

	//multiple colliders to test with
	auto pscolCollider = std::make_shared<CSphereCollider>(&(this->m_vec3Position));
	pscolCollider->m_fRadius = 1;

	vec3 vec3Extents = { 2.f,2.f,3.5f };
	auto paabcolCollider = std::make_shared<CAxisAlignedBoxCollider>(&(this->m_vec3Position), vec3Extents);

	auto pobcolColldider = std::make_shared<COrientatedBox>(&(this->m_vec3Position), &prbCurrent->m_vec3TotalRotationForce, vec3Extents);

	this->m_pcolCollider = pobcolColldider;

#pragma endregion
}

CPlayer::~CPlayer(){
}

void CPlayer::ProgramStart() {
}

void CPlayer::ProgramUpdate()
{
	vec3 vec3Force, vec3ForcePoint;

	if (CInput::m_mKeys['w']){
		vec3Force = { 0, 0, -10 };
		vec3Force *= CGameObject::sm_fDeltaTime * 1000;
		vec3ForcePoint = { 0,0,-1 };
		this->m_pbdPhysicalBody->AddForce(vec3Force, vec3ForcePoint);
		
	}

	if (CInput::m_mKeys['s']){
		vec3Force = {0, 0, 10 };
		vec3ForcePoint = { 0, 0, 1 };
		vec3Force *= CGameObject::sm_fDeltaTime * 1000;
		this->m_pbdPhysicalBody->AddForce(vec3Force, vec3ForcePoint);
	}

	if(CInput::m_mKeys['e'])
	{
		vec3Force = { -3.5f, 0, 0 };
		vec3ForcePoint = { 1, 0, 1 };
		vec3Force *= CGameObject::sm_fDeltaTime * 1000;
		this->m_pbdPhysicalBody->AddForce(vec3Force, vec3ForcePoint);
	}

	if (CInput::m_mKeys['q'])
	{
		vec3Force = { 3.5f, 0, 0 };
		vec3ForcePoint = { 1, 0, 1 };
		vec3Force *= CGameObject::sm_fDeltaTime * 1000;
		this->m_pbdPhysicalBody->AddForce(vec3Force, vec3ForcePoint);
	}

	//uncomment for better control
	
	if (CInput::m_mKeys['d'])
	{
		vec3Force = { 5, 0, 0 };
		vec3ForcePoint = { 1, 0, 0 };
		vec3Force *= CGameObject::sm_fDeltaTime * 1000;
		this->m_pbdPhysicalBody->AddForce(vec3Force, vec3ForcePoint);
	}

	if (CInput::m_mKeys['a'])
	{
		vec3Force = { -5, 0, 0 };
		vec3ForcePoint = { -1, 0, 0 };
		vec3Force *= CGameObject::sm_fDeltaTime * 1000;
		this->m_pbdPhysicalBody->AddForce(vec3Force, vec3ForcePoint);
	}	

	std::cout << "Update total force: W" << glm::to_string(this->m_pbdPhysicalBody->m_vec3TotalLineraForce) << " R:" << glm::to_string(this->m_pbdPhysicalBody->m_ppbReadable->m_vec3TotalLineraForce) << "\n";
}

void CPlayer::Draw()
{	
	unsigned int uiTextureLocation = glGetUniformLocation(*CGameObject::sm_puiProgramID, "Texture2D");
	glUniform1i(uiTextureLocation, this->m_uiTexturePositionOffset);

	glBindVertexArray(this->m_uiVertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, this->m_pobjrGeometry->numVertices);

	if (*CGameObject::sm_pbDebugMode) {
		unsigned int uiProgramID = *CGameObject::sm_puiProgramID;
		glUniform1ui(glGetUniformLocation(uiProgramID, "uiVisualizationType"), (int)CGameObject::EVisualizationType::NO_DATA);
		this->m_pcolCollider->DrawShape();
	}
}

CGameObject* CPlayer::Clone()
{
	CPlayer* pplClone = new CPlayer(*this);

	//create all other pointers like rotating body and similar
	CRotattingBody* prbClonedPhyiscs = new CRotattingBody(*static_cast<CRotattingBody*>(this->m_pbdPhysicalBody));
	
	prbClonedPhyiscs->m_ppbReadable = this->m_pbdPhysicalBody;

	this->m_pbdPhysicalBody->m_ppbReadable = prbClonedPhyiscs;

	pplClone->m_pbdPhysicalBody = prbClonedPhyiscs;


	return pplClone;
}
