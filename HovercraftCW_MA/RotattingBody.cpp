#include "RotattingBody.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

CRotattingBody::CRotattingBody(){
	this->m_fAngularDamp = 0;
}

CRotattingBody::~CRotattingBody()
{
}

void CRotattingBody::AddForce(vec3& rvec3Force, vec3& rvec3RelativePosition, CPhysicalBody::EForceType eftForce){
	if (eftForce == CRotattingBody::EForceType::Input)
	{
		float fSinX = glm::sin(glm::radians(this->m_vec3Orientation.x));
		float fCosX = glm::cos(glm::radians(this->m_vec3Orientation.x));

		float fSinY = glm::sin(glm::radians(this->m_vec3Orientation.y));
		float fCosY = glm::cos(glm::radians(this->m_vec3Orientation.y));

		float fSinZ = glm::sin(glm::radians(this->m_vec3Orientation.z));
		float fCosZ = glm::cos(glm::radians(this->m_vec3Orientation.z));

		glm::mat3 mat3AroundX(
			1, 0, 0,
			0, fCosX, -fSinX,
			0, fSinX, fCosX);

		glm::mat3 mat3AroundZ(
			fCosZ, -fSinZ, 0,
			fSinZ, fCosZ, 0,
			0, 0, 1);

		glm::mat3 mat3AroundY(
			fCosY, 0, fSinY,
			0, 1, 0,
			-fSinY, 0, fCosY);
		
		this->m_vec3TotalLineraForce += (rvec3Force * this->m_fMass);
		this->m_vec3TotalRotationForce += glm::cross(rvec3RelativePosition, rvec3Force);
		this->m_vec3TotalLineraForce = this->m_vec3TotalLineraForce * mat3AroundY * mat3AroundX * mat3AroundZ;
	}
	else {
		this->m_vec3TotalLineraForce += (rvec3Force * this->m_fMass);
	}
}
