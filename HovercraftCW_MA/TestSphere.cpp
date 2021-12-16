#include "TestSphere.h"


CTestSphere::CTestSphere(SMaterial* pmatrMaterial) : CGameObject(&(string)"../Geometry/Sphere.obj", nullptr, pmatrMaterial )
{
	this->m_strName = "Test sphere";

	this->m_tsTag.m_vstrHeldTags.emplace_back("Collides");


#pragma region Movement
	CRotattingBody* prbCurrent = new CRotattingBody();

	prbCurrent->m_bIsAffectedByGravity = false;

	float fMass = 3;
	prbCurrent->m_fMass = fMass;
	prbCurrent->m_fInvertedMass = 1 / fMass;
	prbCurrent->m_fVelocityDamp = 0.6f;

	prbCurrent->m_mat3Inertia = mat3(
		fMass * 0.0833333333333333f * 9, 0, 0,
		0, fMass * 0.0833333333333333f * 9, 0,
		0, 0, fMass * 0.0833333333333333f * 9);


	prbCurrent->m_mat3InvertedInertia = glm::inverse(prbCurrent->m_mat3Inertia);

	prbCurrent->m_fAngularDamp = 0.5f;


	this->m_pbdPhysicalBody = prbCurrent;
#pragma endregion

#pragma region Colliders

	auto pscolCollider = std::make_shared<CSphereCollider>(&(this->m_vec3Position));
	pscolCollider->m_fRadius = 1;

	vec3 vec3Extents = { 1,1,1 };

	auto paabcolCollider = std::make_shared<CAxisAlignedBoxCollider>(&(this->m_vec3Position), vec3Extents);

	auto porbCollider = std::make_shared<COrientatedBox>(&this->m_vec3Position, &prbCurrent->m_vec3Orientation, vec3Extents);

	this->m_pcolCollider = pscolCollider;
	//this->m_bIsStatic = true;
#pragma endregion
}

CTestSphere::~CTestSphere(){
}

void CTestSphere::ProgramStart(){
	this->m_vec3Position.x += 5;
	//Enable this to see collision response on more than one axis
	//this->m_vec3Position.y += 1;
}

void CTestSphere::ProgramUpdate()
{
}
