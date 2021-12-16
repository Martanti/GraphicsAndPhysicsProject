#include "InvisiblePlane.h"

CInvisiblePlane::CInvisiblePlane() {
	CInvisiblePlane({ 0.f,0.f,0.f }, {1,0,0});
}
CInvisiblePlane::CInvisiblePlane(vec3 vec3Position, vec3 vec3Normal) {
	this->m_vec3Position = vec3Position;
	auto ppcolCollider = std::make_shared<CPlaneCollider>(&(this->m_vec3Position), vec3Normal);

	this->m_pcolCollider = ppcolCollider;
	this->m_bIsStatic = true;

	this->m_tsTag.m_vstrHeldTags.emplace_back("Collides");
}

CInvisiblePlane::~CInvisiblePlane()
{
}

void CInvisiblePlane::ProgramStart()
{
}

void CInvisiblePlane::ProgramUpdate()
{
}

void CInvisiblePlane::Draw()
{
	if (*CGameObject::sm_pbDebugMode) {
		glUniform1ui(glGetUniformLocation(*CGameObject::sm_puiProgramID, "uiVisualizationType"), (int)CGameObject::EVisualizationType::NO_DATA);
		this->m_pcolCollider->DrawShape();
	}
}
