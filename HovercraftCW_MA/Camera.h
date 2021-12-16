#include <glm/glm.hpp>
#include "GameObject.h"
#include "RotattingBody.h"

using glm::vec3;

#pragma once

class CCamera : public CGameObject
{
public:
	
	CCamera();
	~CCamera();

	//A function pointer to appropriate camera calculations
	static void (*funcpCalculateCamera)(CCamera & rcamMain, CGameObject& rgoObservable);

	//position is the eye

	vec3 m_vec3Center;
	vec3 m_vec3Up;

	void ProgramStart();
	void ProgramUpdate();

	CGameObject* Clone();

	void Draw();

	static const vec3 sm_kvec3SplashScreenPosition;

private:
	enum class ECameraType { ThirdPerson, TopToDown, FixedPoint, FreeMove, SplashScreen };
	typedef void (*CameraCalculation)(CCamera& rcamMain, CGameObject& rgoObservable);

	static const vec3 sm_kvec3FixedPoint;
	static bool sm_bHasSplashScreenBeenClosed;
	static const vec3 sm_kvec3SplashScreenCameraPosition;
	static const vec3 sm_kvec3GeneralUp;

	static void ChangeCameraCalculation(ECameraType rcamTypeToSet);

	static void CalculateThirdPerson(CCamera& rcamMain, CGameObject& rgoObservable);
	static void CalculateTopToDown(CCamera& rcamMain, CGameObject& rgoObservable);
	static void CalculateFixedPoint(CCamera& rcamMain, CGameObject& rgoObservable);
	static void CalculateSplashScreen(CCamera& rcamMain, CGameObject& rgoObservable);

};

