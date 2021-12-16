#include "PhysicsCalculator.h"
#include <iostream>
#include <glm\gtx\string_cast.hpp>

const float CPhysicsCalculator::m_ksfGravity = -9.81f;
//default value
CPhysicsCalculator::EIntegrationType CPhysicsCalculator::CurrentIntegration = CPhysicsCalculator::EIntegrationType::ExplicitEulers;

CPhysicsCalculator::CPhysicsCalculator(){
	funcpLinearCalculation = &(CPhysicsCalculator::ExplicitEulersLinearCalculation);
	funcpRotationalCalculation = &(CPhysicsCalculator::ExplicitEulersRotationalCalculation);
}

CPhysicsCalculator::~CPhysicsCalculator(){
}

void CPhysicsCalculator::RecalculatePhysics(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadable, float & rfDeltaTime)
{
	int iIndex = 0;
	for (CGameObject * pgoReadableItem : rvgoReadable)
	{

		if (pgoReadableItem->m_pbdPhysicalBody != nullptr && pgoReadableItem->m_pbdPhysicalBody->m_fMass > 0)
		{
			vec3 vec3Acceleration, vec3Velocity, vec3Position;

			CGameObject* pgoWritableItem = rvgoWritable[iIndex];

			pgoReadableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce = pgoWritableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce;
			funcpLinearCalculation(*pgoReadableItem, vec3Acceleration, vec3Velocity, vec3Position, rfDeltaTime);

			pgoWritableItem->m_pbdPhysicalBody->m_vec3Acceleration = vec3Acceleration;
			pgoWritableItem->m_pbdPhysicalBody->m_vec3Velocity = vec3Velocity;
			pgoWritableItem->m_vec3Position = vec3Position;

			if (pgoReadableItem->m_strName =="Player"){
				//std::cout<<"Pos: "<<glm::to_string(vec3Position)<<" Vel: " << glm::to_string(vec3Velocity)<< " Acc: "<<glm::to_string(vec3Acceleration)<<" TotalLinear:"<< glm::to_string(pgoReadableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce)<<" delta: "<<rfDeltaTime<<"\n";
				//std::cout << "Total force during physics: W: " << glm::to_string(pgoWritableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce) << " R:" << glm::to_string(pgoReadableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce) << "\n";
			}


			CRotattingBody* prbCurrent = static_cast<CRotattingBody*>(pgoReadableItem->m_pbdPhysicalBody);
			if (prbCurrent != nullptr) {

				vec3 vec3AngularAcceleration, vec3AngularVelocity, vec3Orientation;

				CRotattingBody* prbWritable = static_cast<CRotattingBody*>(pgoWritableItem->m_pbdPhysicalBody);

				prbCurrent->m_vec3TotalRotationForce = prbWritable->m_vec3TotalRotationForce;
				funcpRotationalCalculation(*prbCurrent, vec3AngularAcceleration, vec3AngularVelocity, vec3Orientation, rfDeltaTime);

				this->Limit3DOrientation(vec3Orientation);

				prbWritable->m_vec3AngularAcceleration = vec3AngularAcceleration;
				prbWritable->m_vec3AngularVelocity = vec3AngularVelocity;
				prbWritable->m_vec3Orientation = vec3Orientation;
			}
		}
		iIndex++;
	}
	
	this->Damping(rvgoWritable, rvgoReadable, rfDeltaTime);
}

void CPhysicsCalculator::ChangeCurrentIntegration(CPhysicsCalculator::EIntegrationType itType)
{
	CPhysicsCalculator::CurrentIntegration = itType;
	this->SetRightCalculations(funcpLinearCalculation, funcpRotationalCalculation);
}

void CPhysicsCalculator::SetRightCalculations(LinearPhysicsCalculation& rpcLinear, RotationalPhysicsCalculation2D& rpcRotational)
{
	//iterate through all the objects and then calculate their velocity and 
	switch (CPhysicsCalculator::CurrentIntegration)
	{

	case (CPhysicsCalculator::EIntegrationType::VelocityVerlet):
		rpcLinear = &(CPhysicsCalculator:: VelocityVerletLinearCalculation);
		rpcRotational = &(CPhysicsCalculator::VelocityVerletRotationalCalculation);

		break;

	case (CPhysicsCalculator::EIntegrationType::SemiExplicitEulers):
		rpcLinear = &(CPhysicsCalculator::SemiExplicitEulerLinearCalculation);
		rpcRotational = &(CPhysicsCalculator::SemiExplicitEulerRotationalCalculation);
		break;

	case(CPhysicsCalculator::EIntegrationType::ExplicitEulers):
	default:
		rpcLinear = &(CPhysicsCalculator::ExplicitEulersLinearCalculation);
		rpcRotational = &(CPhysicsCalculator::ExplicitEulersRotationalCalculation);
		break;
	}
}

#pragma region Calculations

void CPhysicsCalculator::ExplicitEulersLinearCalculation(CGameObject& rgoItem, vec3& rvec3Acceleration, vec3& rvec3Velocity, vec3& rvec3Position, float& rfDeltaTime)
{
	rvec3Acceleration = rgoItem.m_pbdPhysicalBody->m_vec3TotalLineraForce * rgoItem.m_pbdPhysicalBody->m_fInvertedMass;
	rvec3Velocity = rgoItem.m_pbdPhysicalBody->m_vec3Velocity + rvec3Acceleration * rfDeltaTime;
	rvec3Position = rgoItem.m_vec3Position + rgoItem.m_pbdPhysicalBody->m_vec3Velocity * rfDeltaTime;
}

void CPhysicsCalculator::ExplicitEulersRotationalCalculation(CRotattingBody& rrbItem, vec3& rvec3AngularAcceleration, vec3& rvec3AngularVelocity, vec3& rvec3Orientation, float& rfDeltaTime)
{
	rvec3AngularAcceleration = rrbItem.m_vec3TotalRotationForce * rrbItem.m_mat3InvertedInertia;
	rvec3AngularVelocity = rrbItem.m_vec3AngularVelocity + rvec3AngularAcceleration * rfDeltaTime;
	rvec3Orientation = rrbItem.m_vec3Orientation + glm::degrees(rrbItem.m_vec3AngularVelocity * rfDeltaTime);
}

void CPhysicsCalculator::SemiExplicitEulerLinearCalculation(CGameObject& rgoItem,vec3& rvec3Acceleration, vec3& rvec3Velocity, vec3& rvec3Position, float& rfDeltaTime)
{
	//uses future time velocity for calculation
	rvec3Acceleration = rgoItem.m_pbdPhysicalBody->m_vec3TotalLineraForce * rgoItem.m_pbdPhysicalBody->m_fInvertedMass;
	rvec3Velocity = rgoItem.m_pbdPhysicalBody->m_vec3Velocity + rvec3Acceleration * rfDeltaTime;
	rvec3Position = rgoItem.m_vec3Position + rvec3Velocity * rfDeltaTime;
}

void CPhysicsCalculator::SemiExplicitEulerRotationalCalculation(CRotattingBody& rrbItem, vec3& rvec3AngularAcceleration, vec3& rvec3AngularVelocity, vec3& rvec3Orientation, float& rfDeltaTime)
{
	rvec3AngularAcceleration = rrbItem.m_vec3TotalRotationForce * rrbItem.m_mat3InvertedInertia;

	rvec3AngularVelocity = rrbItem.m_vec3AngularVelocity + rvec3AngularAcceleration * rfDeltaTime;

	rvec3Orientation = rrbItem.m_vec3Orientation + glm::degrees(rvec3AngularVelocity * rfDeltaTime);
}

void CPhysicsCalculator::VelocityVerletLinearCalculation(CGameObject& rgoItem, vec3& rvec3Acceleration, vec3& rvec3Velocity, vec3& rvec3Position, float& rfDeltaTime)
{
	rvec3Acceleration = rgoItem.m_pbdPhysicalBody->m_vec3TotalLineraForce * rgoItem.m_pbdPhysicalBody->m_fInvertedMass;

	rvec3Position = rgoItem.m_vec3Position+  rgoItem.m_pbdPhysicalBody->m_vec3Velocity * rfDeltaTime +  rvec3Acceleration * 0.5f * rfDeltaTime * rfDeltaTime;
	rvec3Velocity = rgoItem.m_pbdPhysicalBody->m_vec3Velocity + rvec3Acceleration * rfDeltaTime;
}

void CPhysicsCalculator::VelocityVerletRotationalCalculation(CRotattingBody& rrbItem, vec3& rvec3AngularAcceleration, vec3& rvec3AngularVelocity, vec3& rvec3Orientation, float& rfDeltaTime)
{
	rvec3AngularAcceleration = rrbItem.m_vec3TotalRotationForce * rrbItem.m_mat3InvertedInertia;

	rvec3Orientation = rrbItem.m_vec3Orientation + glm::degrees(rrbItem.m_vec3AngularVelocity * rfDeltaTime + rvec3AngularAcceleration * 0.5f * rfDeltaTime * rfDeltaTime);
	rvec3AngularVelocity = rrbItem.m_vec3AngularVelocity + rvec3AngularAcceleration * rfDeltaTime;
}
#pragma endregion

void CPhysicsCalculator::Damping(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadable, float& rfDeltaTime)
{
	//Damp Calculations
	int iIndex = 0;
	for (CGameObject* pgoReadableItem : rvgoReadable) {
		if (pgoReadableItem->m_pbdPhysicalBody != nullptr)
		{

			//damp the velocity

			CGameObject* pgoWritableItem = rvgoWritable[iIndex];

			pgoWritableItem->m_pbdPhysicalBody->m_vec3Velocity *= powf(pgoReadableItem->m_pbdPhysicalBody->m_fVelocityDamp, rfDeltaTime);

			if (pgoReadableItem->m_pbdPhysicalBody->m_vec3Velocity != vec3(0.f,0.f,0.f) && glm::length(pgoReadableItem->m_pbdPhysicalBody->m_vec3Velocity) < 0.01)
			{
				pgoWritableItem->m_pbdPhysicalBody->m_vec3Velocity = { 0,0,0 };
			}

			//reset all the forces for the next frame as the input has already been calculated
			pgoWritableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce = { 0, 0, 0 };
			if (pgoReadableItem->m_strName == "Player")
			{
				std::cout << "Total force after damp: W: " << glm::to_string(pgoWritableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce)<<" R:" <<glm::to_string(pgoReadableItem->m_pbdPhysicalBody->m_vec3TotalLineraForce)<<"\n";
			}

			//damp rotations
			CRotattingBody* prbReadable = static_cast<CRotattingBody*>(pgoReadableItem->m_pbdPhysicalBody);

			if (prbReadable != nullptr) {

				CRotattingBody* prbWritable = static_cast<CRotattingBody*>(pgoWritableItem->m_pbdPhysicalBody);

				prbWritable->m_vec3AngularVelocity *= powf(prbReadable->m_fAngularDamp, rfDeltaTime);

				if (prbReadable->m_vec3AngularVelocity != vec3(0.f, 0.f, 0.f) && glm::length(prbReadable->m_vec3AngularVelocity) < 0.01)
				{
					prbWritable->m_vec3AngularVelocity = { 0,0,0 };
				}
				prbWritable->m_vec3TotalRotationForce = { 0, 0, 0 };
			}
		}

		iIndex++;
	}
}

void CPhysicsCalculator::Limit3DOrientation(vec3 & rvec3Orientation)
{
	//x
	if (rvec3Orientation.x >= 360) {
		rvec3Orientation.x -= 360;
	}
	else if (rvec3Orientation.x < 0) {
		rvec3Orientation.x += 360;
	}

	//y
	if (rvec3Orientation.y >= 360) {
		rvec3Orientation.y -= 360;
	}
	else if (rvec3Orientation.y < 0) {
		rvec3Orientation.y += 360;
	}

	//z
	if (rvec3Orientation.z >= 360) {
		rvec3Orientation.z -= 360;
	}
	else if (rvec3Orientation.z < 0) {
		rvec3Orientation.z += 360;
	}
}