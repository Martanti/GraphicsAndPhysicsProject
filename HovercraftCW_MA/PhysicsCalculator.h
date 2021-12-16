
#pragma region Includes
#include "GameObject.h"
#include "RotattingBody.h"
#include "Time.h"
#include <vector>
#include <chrono>
#pragma endregion

#pragma once

using std::vector;
class CPhysicsCalculator
{
public:

	CPhysicsCalculator();
	~CPhysicsCalculator();

	enum class EIntegrationType
	{
		ExplicitEulers = 0,
		SemiExplicitEulers = 1,
		VelocityVerlet = 2
	};

	void RecalculatePhysics(vector<CGameObject*> &rvgoWritable, vector<CGameObject*> &rvgoReadable, float & rfDeltaTime);

	const static float m_ksfGravity;
	
	void ChangeCurrentIntegration(CPhysicsCalculator::EIntegrationType itType);

	bool* m_pbDebugMode;

	static EIntegrationType CurrentIntegration;

private:

	void (*funcpLinearCalculation)(CGameObject& rgoItem, vec3& rv3Acceleration, vec3& rv3Velocity, vec3& rv3Position, float& rfDeltaTime);
	void (*funcpRotationalCalculation)(CRotattingBody& rgoItem, vec3& rv3AngularAcceleration, vec3& rv3AngularVelocity, vec3& rvec3Orientation, float& rfDeltaTime);

	typedef void (*LinearPhysicsCalculation)(CGameObject & rgoItem ,vec3& rv3Acceleration, vec3& rv3Velocity, vec3& rv3Position, float& rfCurrentTime);

	typedef void (*RotationalPhysicsCalculation2D)(CRotattingBody& rrbItem, vec3 & rv3AngularAcceleration, vec3 & rv3AngularVelocity, vec3 & rvec3Orientation, float &rfCurrentTime );

	void SetRightCalculations(LinearPhysicsCalculation & rpcLinear, RotationalPhysicsCalculation2D & rpcRotational);

	/*
	 *	Explicit Eulers
	 */

	static void ExplicitEulersLinearCalculation(CGameObject& rgoItem, vec3 & rv3Acceleration, vec3 & rv3Velocity, vec3 & rv3Position,float & rfCurrentTime);
	static void ExplicitEulersRotationalCalculation(CRotattingBody& rrbItem, vec3& rv3AngularAcceleration, vec3& rv3AngularVelocity, vec3& rvec3Orientation, float& rfCurrentTime);

	/*
	 *	Semi Explicit Eulers
	 */

	static void SemiExplicitEulerLinearCalculation(CGameObject& rgoItem, vec3& rv3Acceleration, vec3& rv3Velocity, vec3& rv3Position, float& rfCurrentTime);
	static void SemiExplicitEulerRotationalCalculation(CRotattingBody& rrbItem, vec3& rv3AngularAcceleration, vec3& rv3AngularVelocity, vec3& rvec3Orientation, float& rfCurrentTime);

	/*
	 *	Velocity Varlet
	 */

	static void VelocityVerletLinearCalculation(CGameObject& rgoItem, vec3& rv3Acceleration, vec3& rv3Velocity, vec3& rv3Position, float& rfCurrentTime);
	static void VelocityVerletRotationalCalculation(CRotattingBody& rrbItem, vec3& rv3AngularAcceleration, vec3& rv3AngularVelocity, vec3& rvec3Orientation, float& rfCurrentTime);

	/*
	 *	Helpers
	 */

	void Damping(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadable, float& rfCurrentTime);

	void Limit3DOrientation(vec3 &rvec3Orientation);

	
};

