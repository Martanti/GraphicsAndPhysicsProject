#include "PhysicalBody.h"

CPhysicalBody::CPhysicalBody()
{
	float fMass = 0;
	vec3 vec3TempAcceleration (0,0,0);
	vec3 vec3TempVelocity(0, 0, 0);
	vec3 vec3TempTotalForce(0, 0, 0);
	CPhysicalBody(fMass, vec3TempAcceleration, vec3TempVelocity, vec3TempTotalForce);
}

CPhysicalBody::CPhysicalBody(float& rfMass)
{
	vec3 vec3TempAcceleration(0, 0, 0);
	vec3 vec3TempVelocity(0, 0, 0);
	vec3 vec3TempTotalForce(0, 0, 0);
	CPhysicalBody(rfMass, vec3TempAcceleration, vec3TempVelocity, vec3TempTotalForce);
}

CPhysicalBody::CPhysicalBody(float& rfMass, vec3& rvec3Acceleration)
{
	vec3 vec3TempVelocity(0, 0, 0);
	vec3 vec3TempTotalForce(0, 0, 0);
	CPhysicalBody(rfMass, rvec3Acceleration, vec3TempVelocity, vec3TempTotalForce);
}

CPhysicalBody::CPhysicalBody(float& rfMass, vec3& rvec3Acceleration, vec3& rvec3Velocity)
{
	vec3 vec3TempTotalForce(0, 0, 0);
	CPhysicalBody(rfMass, rvec3Acceleration, rvec3Velocity, vec3TempTotalForce);
}

CPhysicalBody::CPhysicalBody(float& rfMass, vec3& rvec3Acceleration, vec3& rvec3Velocity, vec3& rvec3TotalForce)
{
	this->m_fMass = rfMass;
	this->m_vec3Acceleration = rvec3Acceleration;
	this->m_vec3Velocity = rvec3Velocity;
	this->m_vec3TotalLineraForce = rvec3TotalForce;
	this->m_fInvertedMass = 1 / this->m_fMass;
}

void CPhysicalBody::AddForce(vec3& rvec3Force, vec3& rvec3RelativePosition, CPhysicalBody::EForceType eftForce){
	this->m_vec3TotalLineraForce += (rvec3Force * this->m_fMass);
}
