#include <glm/glm.hpp>

#pragma once

using glm::vec3;

class CPhysicalBody
{
public:
	enum class EForceType { Input, Enviromental };

	CPhysicalBody();
	CPhysicalBody(float& rfMass);
	CPhysicalBody(float& rfMass, vec3& rv3Acceleration);
	CPhysicalBody(float& rfMass, vec3& rv3Acceleration, vec3& rv3Velocity);
	CPhysicalBody(float &rfMass, vec3 &rv3Acceleration, vec3& rv3Velocity, vec3 &rv3TotalForce);
	~CPhysicalBody() = default; 

	float m_fMass;
	// Precalculated 1/mass for later calculation. If mass is changed one needs to update this as well
	float m_fInvertedMass;
	vec3 m_vec3Acceleration;
	vec3 m_vec3Velocity;
	vec3 m_vec3TotalLineraForce;

	//How quickly should the velocity be reduced to zero
	//0 will stop immediately and 1 will result in no damping
	float m_fVelocityDamp = 0.5f;

	bool m_bIsAffectedByGravity = true;

	virtual void AddForce(vec3& rvec3Force, vec3& rvec3RelativePosition, CPhysicalBody::EForceType eftForce = CPhysicalBody::EForceType::Input);
};

