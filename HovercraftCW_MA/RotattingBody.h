
#include "PhysicalBody.h"

#pragma once

using glm::mat3;
class CRotattingBody : public CPhysicalBody
{
public:

	

	mat3 m_mat3Inertia; //for 3d stuff

	mat3 m_mat3InvertedInertia;

	vec3 m_vec3AngularAcceleration;
	vec3 m_vec3AngularVelocity;

	vec3 m_vec3Orientation;
	
	float m_fAngularDamp;

	vec3 m_vec3TotalRotationForce;

	CRotattingBody();
	~CRotattingBody();

	void AddForce(vec3& rvec3Force, vec3& rvec3RelativePosition, CPhysicalBody::EForceType eftForce = CPhysicalBody::EForceType::Input);

};

