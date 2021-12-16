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

	if (this->m_ppbReadable != nullptr)
	{
		this->m_vec3TotalLineraForce += (rvec3Force * this->m_fMass);

		if (eftForce == CRotattingBody::EForceType::Input)
		{
			CRotattingBody* prbReadable = static_cast<CRotattingBody*>(this->m_ppbReadable);

			vec3 vec3ReadableOrientation = prbReadable->m_vec3Orientation;

			float fSinX = glm::sin(glm::radians(vec3ReadableOrientation.x));
			float fCosX = glm::cos(glm::radians(vec3ReadableOrientation.x));

			float fSinY = glm::sin(glm::radians(vec3ReadableOrientation.y));
			float fCosY = glm::cos(glm::radians(vec3ReadableOrientation.y));

			float fSinZ = glm::sin(glm::radians(vec3ReadableOrientation.z));
			float fCosZ = glm::cos(glm::radians(vec3ReadableOrientation.z));

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


			//make a refernece "readable" upon cloning and use it here to build instead of reading itself

			//this->m_vec3TotalLineraForce = prbReadable->m_vec3TotalLineraForce + (rvec3Force * this->m_fMass);
			this->m_vec3TotalRotationForce +=  glm::cross(rvec3RelativePosition, rvec3Force);

			//it rotates held linear force
			this->m_vec3TotalLineraForce = this->m_vec3TotalLineraForce * mat3AroundY * mat3AroundX * mat3AroundZ;
		}
	}
}
