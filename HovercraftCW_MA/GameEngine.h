
#pragma region includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Input.h"
#include "GameObject.h"
#include "Time.h"
#include "PhysicsCalculator.h"
#include "Player.h"
#include "Skybox.h"
#include "InvisiblePlane.h"

#include "RotattingBody.h"

#include <vector>

#include <iostream>
#include <string>

#include "getbmp.h"
#include "objReader.h"
#include "shader.h"
#include "vertex.h"

#include "Light.h"
#include "Camera.h"
#include "WindyArea.h"
#include "AutoMovingObject.h"

#include "CollisionCalculator.h"
#include "TestSphere.h"

#include <thread>
#include <mutex>

#pragma comment(lib, "glew32.lib") 
#pragma endregion

using std::vector;
using std::string;

#pragma once
class CGameEngine
{
public:

	static bool sm_bDebugMode;

	static unsigned int sm_uiProgramID;

	CGameEngine();
	~CGameEngine();

	void InitEngine(int argc, char** argv, const char* pcTitle, int & riWidth, int& riHeight);
	void AddGameObject(CGameObject* pgoObject);
	void StartEngine();

private:

	/*
	 *	OPENGL MODERN
	 */
	
	static unsigned int sm_uiVertexShaderID;
	static unsigned int sm_uiFragmentShaderID;

	//A link between backend and shaders

	static unsigned int sm_uiProjectionMatrixID;
	static glm::mat4 sm_mat4ProjectionMatrix;

	static unsigned int sm_uiViewID;
	static glm::mat4 sm_mat4View;

	static unsigned int sm_uiModelID;
	static glm::mat4 sm_mat4Model;

	/*
	 *	BACKEND	
	 */

	static float sm_fDeltaTime;

	static CPhysicsCalculator sm_phcaPhysicsCalculator;

	static CCollisionCalculator sm_colcaCollisionCalculator;

	static void DrawScene();
	static void UpdateGame();
	static void CleanupGame();

	static void BeginGameLoop();

	static void ResizeWindow(int iWidth, int iHeight);

	/*
	 *	For Managing the systems
	 */

	//Check for inputs that can/will affect system behaviour
	static void SystemManagement();


#pragma region Inputs
	// Keyboard input processing routine.
	static void KeyDown(unsigned char ucKey, int iX, int iY);

	static void KeyUp(unsigned char ucKey, int iX, int iY);

	static void KeySpecialDown(int iKeyCode, int x, int y);

	static void KeySpecialUp(int iKeyCode, int x, int y);

#pragma endregion Inputs

#pragma region Multithreading

	static bool sm_bContinueThreads;

	static void CalculatePhysics();
	static void CalculateCollision();
	static void CalculateUpdate();

	static void UpdateDataBuffers();

	static unsigned int sm_uiWritingBuffer;
	static unsigned int sm_uiReadingBuffer;

	static const unsigned int sm_kuiBufferAmount = 2;

	static vector<CGameObject*>  sm_rvpgoBuffers[sm_kuiBufferAmount];

	static std::thread sm_thrPhysics;
	static std::thread sm_thrUpdate;
	static std::thread sm_thrCollisions;
	static std::thread sm_thrSystemUpdates;
#pragma endregion
};