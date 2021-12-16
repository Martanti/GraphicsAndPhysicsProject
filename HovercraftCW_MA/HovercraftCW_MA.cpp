#include "GameEngine.h"

int main(int argc, char** argv)
{
	int iWindowLength = 1600;
	int iWindowHeight = 800;

	CGameEngine geEngine;

	geEngine.InitEngine(argc, argv, "Cool hovercraft game", iWindowLength, iWindowHeight);

#pragma region Default Material
	SMaterial matrDefault;
	matrDefault.m_vec3Emission = { 0,0,0 };
	matrDefault.m_vec3Ambient = { 0.3f,0.3f,0.3f };
	matrDefault.m_vec3Diffuse = { 1,1,1 };
	matrDefault.m_fGloss = 50;
	matrDefault.m_vec3Specular = { 1,1,1 };
#pragma endregion

#pragma region Hovercraft
	CPlayer* pcoBox = new CPlayer(&matrDefault);
	geEngine.AddGameObject(pcoBox);
#pragma endregion

#pragma region Directional Light
	CLight* plDirectionalLight = new CLight();
	plDirectionalLight->m_strName = "Light source";
	geEngine.AddGameObject(plDirectionalLight);
#pragma endregion	

#pragma region Camera
	CCamera* pcamMain = new CCamera();
	geEngine.AddGameObject(pcamMain);


	CGameObject* pgoSplashScreen = new CGameObject(&(string)"../Geometry/plane.obj", &(string)"../Geometry/Splashscreen.jpg");
	pgoSplashScreen->m_vec3Position = CCamera::sm_kvec3SplashScreenPosition;
	//start with splash screen

	CRotattingBody* prbSplashScreenRotatingBody = new CRotattingBody();
	prbSplashScreenRotatingBody->m_vec3Orientation = {90,0,0};
	pgoSplashScreen->m_vec3Scale = { 2,2,2 };
	pgoSplashScreen->m_pbdPhysicalBody = prbSplashScreenRotatingBody;
	pgoSplashScreen->m_strName = "Splash screen";
	geEngine.AddGameObject(pgoSplashScreen);
#pragma endregion

#pragma region Test sphere
	SMaterial matrTestSphere;

	matrTestSphere.m_vec3Emission = { 0,0,0};
	matrTestSphere.m_vec3Ambient = { 0.3f,0.3f,0.0f };
	matrTestSphere.m_vec3Diffuse = { 1,1,0 };
	matrTestSphere.m_fGloss = 20;
	matrTestSphere.m_vec3Specular = { 1,1,1 };

	CTestSphere* ptsSphere = new CTestSphere(&matrTestSphere);
	
	geEngine.AddGameObject(ptsSphere);
#pragma endregion

#pragma region Skybox
	CSkybox* psbSkybox = new CSkybox();
	psbSkybox->m_strName = "Skybox";
	geEngine.AddGameObject(psbSkybox);
#pragma endregion

#pragma region Track
	CGameObject* pgoPlane = new CGameObject(&(string)"../Geometry/Field.obj", &(string)"../Geometry/FieldUV.jpg");
	pgoPlane->m_strName = "Arena";
	pgoPlane->m_vec3Scale = { 2.5,2.5,2.5 };
	geEngine.AddGameObject(pgoPlane);
#pragma endregion
	
#pragma region Signposts
	CGameObject* pgoSign1 = new CGameObject(&(string)"../Geometry/Sign.obj", &(string)"../Geometry/SignUV.jpg");
	pgoSign1->m_strName = "Wind sign 1";
	pgoSign1->m_vec3Position = { -13,0, -4 };
	pgoSign1->m_vec3Scale = { 0.4f,0.4f,0.4f };

	vec3 vec3Extents = { 0.2f, 5.f, 0.2f };

	auto paabcolColider1 = std::make_shared<CAxisAlignedBoxCollider>(&pgoSign1->m_vec3Position, vec3Extents );
	pgoSign1->m_pcolCollider = paabcolColider1;

	pgoSign1->m_tsTag.m_vstrHeldTags.emplace_back("Collides");
	geEngine.AddGameObject(pgoSign1);

	CGameObject* pgoSign2 = new CGameObject(&(string)"../Geometry/Sign.obj", &(string)"../Geometry/SignUV.jpg");
	pgoSign2->m_strName = "Wind sign 2";
	pgoSign2->m_vec3Position = { -28.5f, 0, -12.5f };
	pgoSign2->m_vec3Scale = { 0.4f,0.4f,0.4f };

	auto paabcolColider2 = std::make_shared<CAxisAlignedBoxCollider>(&pgoSign2->m_vec3Position, vec3Extents);
	pgoSign2->m_pcolCollider = paabcolColider2;

	pgoSign2->m_tsTag.m_vstrHeldTags.emplace_back("Collides");
	geEngine.AddGameObject(pgoSign2);

#pragma endregion

#pragma region Car
	CAutoMovingObject* pamoCar = new CAutoMovingObject(&(string)"../Geometry/car.obj", &(string)"../Geometry/CarUV.jpg", &matrDefault);
	pamoCar->m_vec3Position.x = 55;
	pamoCar->m_vec3Scale = { 1.5f,1.5f,1.5f };
	geEngine.AddGameObject(pamoCar);
#pragma endregion

#pragma region Inisible walls
	CInvisiblePlane* pivWall1 = new CInvisiblePlane({ -49,0,0 }, { 1,0,0 });
	pivWall1->m_strName = "Wall 1";
	geEngine.AddGameObject(pivWall1);

	CInvisiblePlane* pivWall2 = new CInvisiblePlane({ 49,0,0 }, { -1,0,0 });
	pivWall2->m_strName = "Wall 2";
	geEngine.AddGameObject(pivWall2);

	CInvisiblePlane* pivWall3 = new CInvisiblePlane({ 0,0,-49 }, { 0,0,1 });
	pivWall3->m_strName = "Wall 3";
	geEngine.AddGameObject(pivWall3);

	CInvisiblePlane* pivWall4 = new CInvisiblePlane({ 0,0, 49 }, { 0,0,-1 });
	pivWall4->m_strName = "Wall 4";
	geEngine.AddGameObject(pivWall4);
#pragma endregion

#pragma region Windy Area
	//this also shows off sphere collider

	CWindyArea* pwaWind = new CWindyArea();
	pwaWind->m_vec3Position = { -20,0,-10 };
	geEngine.AddGameObject(pwaWind);
#pragma endregion

	geEngine.StartEngine();
}