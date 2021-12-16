#include "Camera.h"

#pragma region Static variable
const vec3 CCamera::sm_kvec3FixedPoint = { 0.0, 10.0f, 10.0 };
const vec3 CCamera::sm_kvec3SplashScreenPosition = {100,100,100};
const vec3 CCamera::sm_kvec3GeneralUp = { 0,1,0 };
bool CCamera::sm_bHasSplashScreenBeenClosed = false;
const vec3 CCamera::sm_kvec3SplashScreenCameraPosition = {100,100, 110};
CCamera::CameraCalculation CCamera::funcpCalculateCamera = &(CCamera::CalculateSplashScreen);
#pragma endregion

CCamera::CCamera()
{
	CRotattingBody* prbBody = new CRotattingBody();
	prbBody->m_bIsAffectedByGravity = false;
	prbBody->m_fMass = 0;

	this->m_pbdPhysicalBody = prbBody;

	this->m_strName = "Camera";

	this->m_tsTag.m_vstrHeldTags.emplace_back("Camera");
}
CCamera::~CCamera(){}

void CCamera::ProgramStart() {
}

void CCamera::ProgramUpdate()
{
	//check for input to check integration type
	if (CInput::m_mSpecialKeys[5]){
		CCamera::ChangeCameraCalculation(CCamera::ECameraType::FixedPoint);
		if (!CCamera::sm_bHasSplashScreenBeenClosed)
			CCamera::sm_bHasSplashScreenBeenClosed = true;
	}

	else if (CInput::m_mSpecialKeys[6]){
		CCamera::ChangeCameraCalculation(CCamera::ECameraType::TopToDown);
		if (!CCamera::sm_bHasSplashScreenBeenClosed)
			CCamera::sm_bHasSplashScreenBeenClosed = true;
	}

	else if (CInput::m_mSpecialKeys[7]){
		CCamera::ChangeCameraCalculation(CCamera::ECameraType::ThirdPerson);
		if (!CCamera::sm_bHasSplashScreenBeenClosed)
			CCamera::sm_bHasSplashScreenBeenClosed = true;
	}

	else if (CInput::m_mKeys[' '] && !CCamera::sm_bHasSplashScreenBeenClosed) {
		CCamera::sm_bHasSplashScreenBeenClosed = true;
		CCamera::ChangeCameraCalculation(CCamera::ECameraType::FixedPoint);
	}

}

CGameObject* CCamera::Clone()
{
	CCamera* pcamClone = new CCamera(*this);
	CRotattingBody* prbClone = new CRotattingBody(*(static_cast<CRotattingBody*>(this->m_pbdPhysicalBody)));
	pcamClone->m_pbdPhysicalBody = prbClone;
	return pcamClone;
}

void CCamera::Draw(){//empty statement just so default isn't called and there is no extra computational time
}

void CCamera::ChangeCameraCalculation(ECameraType rcamTypeToSet)
{
	switch (rcamTypeToSet)
	{
	case CCamera::ECameraType::FixedPoint:
		CCamera::funcpCalculateCamera = &(CCamera::CalculateFixedPoint);
		
		break;

	case CCamera::ECameraType::TopToDown:
		CCamera::funcpCalculateCamera = &(CCamera::CalculateTopToDown);
		
		break;

	case CCamera::ECameraType::ThirdPerson:
	default:
		CCamera::funcpCalculateCamera = &(CCamera::CalculateThirdPerson);
		break;
	}
}

void CCamera::CalculateThirdPerson(CCamera& rcamMain, CGameObject& rgoObservable)
{
	float fYRotation = 0;
	CRotattingBody* prbBody = static_cast<CRotattingBody*>(rgoObservable.m_pbdPhysicalBody);
	if (prbBody != nullptr)
	{
		fYRotation = prbBody->m_vec3Orientation.y;
	}

	float fCosAngle = glm::cos(glm::radians(fYRotation));
	float fSinAngle = glm::sin(glm::radians(fYRotation));

	mat3 mat3AroundY(
		fCosAngle, 0, fSinAngle,
		0, 1, 0,
		-fSinAngle, 0, fCosAngle);

	//also rotate position around the player
	rcamMain.m_vec3Up = CCamera::sm_kvec3GeneralUp;

	float fOffset = 5;

	rcamMain.m_vec3Position = rgoObservable.m_vec3Position + vec3(0,fOffset, fOffset)*mat3AroundY;


	rcamMain.m_vec3Center = rgoObservable.m_vec3Position;
	//so camera looks above the player and not dead center
	rcamMain.m_vec3Center.y += 3;
}

void CCamera::CalculateTopToDown(CCamera& rcamMain, CGameObject& rgoObservable)
{
	//so up is the (negative) forward
	rcamMain.m_vec3Up = { 0,0,-1 };

	//rotate by the Y rotation of the player

	rcamMain.m_vec3Position = rgoObservable.m_vec3Position;

	//add an offset
	rcamMain.m_vec3Position.y += 10;

	rcamMain.m_vec3Center = rgoObservable.m_vec3Position;

	CRotattingBody* prbCurrent = static_cast<CRotattingBody*>(rcamMain.m_pbdPhysicalBody);
	CRotattingBody* prbFollowed = static_cast<CRotattingBody*>(rgoObservable.m_pbdPhysicalBody);

	if (prbCurrent != nullptr && prbFollowed != nullptr) {
		prbCurrent->m_vec3Orientation.y = prbFollowed->m_vec3Orientation.y;
	}
}

void CCamera::CalculateFixedPoint(CCamera& rcamMain, CGameObject& rgoObservable)
{
	rcamMain.m_vec3Up = CCamera::sm_kvec3GeneralUp;
	rcamMain.m_vec3Position = CCamera::sm_kvec3FixedPoint;
	rcamMain.m_vec3Center = rgoObservable.m_vec3Position;
}

void CCamera::CalculateSplashScreen(CCamera& rcamMain, CGameObject& rgoObservable)
{
	rcamMain.m_vec3Up = CCamera::sm_kvec3GeneralUp;
	rcamMain.m_vec3Position = CCamera::sm_kvec3SplashScreenCameraPosition;
	rcamMain.m_vec3Center = CCamera::sm_kvec3SplashScreenPosition;
}
