#include "GameEngine.h"
#include <glm\gtx\string_cast.hpp>

#pragma region Static variable initialization
float CGameEngine::sm_fDeltaTime;
CPhysicsCalculator CGameEngine::sm_phcaPhysicsCalculator;
unsigned int CGameEngine::sm_uiProgramID;
unsigned int CGameEngine::sm_uiVertexShaderID;
unsigned int CGameEngine::sm_uiFragmentShaderID;
unsigned int CGameEngine::sm_uiProjectionMatrixID;
glm::mat4 CGameEngine::sm_mat4ProjectionMatrix;
unsigned int CGameEngine::sm_uiViewID;
glm::mat4 CGameEngine::sm_mat4View;
unsigned int CGameEngine::sm_uiModelID;
glm::mat4 CGameEngine::sm_mat4Model;
CCollisionCalculator CGameEngine::sm_colcaCollisionCalculator;

bool CGameEngine::sm_bDebugMode = false;

bool CGameEngine::sm_bContinueThreads = true;

vector<CGameObject*>  CGameEngine::sm_rvpgoBuffers[sm_kuiBufferAmount];

unsigned int sm_uiWritingBuffer;
unsigned int sm_uiReadingBuffer;

const unsigned int sm_kuiBufferAmount = 2;

unsigned int CGameEngine::sm_uiWritingBuffer;
unsigned int CGameEngine::sm_uiReadingBuffer;

std::thread CGameEngine::sm_thrPhysics;
std::thread CGameEngine::sm_thrUpdate;
std::thread CGameEngine::sm_thrCollisions;
std::thread CGameEngine::sm_thrSystemUpdates;
#pragma endregion

CGameEngine::CGameEngine() {}

CGameEngine::~CGameEngine() {}

void CGameEngine::InitEngine(int argc, char** argv, const char* pcTitle, int& riWidth, int& riHeight)
{
	CGameEngine::sm_phcaPhysicsCalculator.m_pbDebugMode = &CGameEngine::sm_bDebugMode;
	CGameObject::sm_pbDebugMode = &CGameEngine::sm_bDebugMode;

	CGameObject::sm_puiProgramID = &CGameEngine::sm_uiProgramID;

	unsigned char ucInitialObjectReserve = 200;

	for (size_t i = 0; i < CGameEngine::sm_kuiBufferAmount; i++)
	{
		CGameEngine::sm_rvpgoBuffers[i].reserve(ucInitialObjectReserve);
	}
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutInitWindowSize(riWidth, riHeight);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(pcTitle);
	glutDisplayFunc(DrawScene);
	glutReshapeFunc(ResizeWindow);

#pragma region Input Calls
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutSpecialFunc(KeySpecialDown);
	glutSpecialUpFunc(KeySpecialUp);
#pragma endregion

	glutIdleFunc(this->UpdateGame);

	glewExperimental = GL_TRUE;
	glewInit();

	//glutCloseFunc(CleanUp);
	atexit(CleanupGame);

	//set the projection matrix so that the screen width and height can be used
	CGameEngine::sm_mat4ProjectionMatrix = glm::perspective(45.0f, (float)(riWidth / riHeight), 0.1f, 1000.0f);
}

void CGameEngine::AddGameObject(CGameObject* pgoObject) {
	CGameEngine::sm_rvpgoBuffers[1].emplace_back(pgoObject);
}

void CGameEngine::StartEngine()
{
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Startup\n";

#pragma region OpenGL
	glClearColor(0.0, 0.53, 0.55, 0.0); //teal background
	glEnable(GL_DEPTH_TEST);

	CGameEngine::sm_uiVertexShaderID = setShader((char*)"vertex", (char*)"vertexShader.glsl");
	CGameEngine::sm_uiFragmentShaderID = setShader((char*)"fragment", (char*)"fragmentShader.glsl");
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Vertex shader ID: " << CGameEngine::sm_uiVertexShaderID << "| Fragment shader ID:  " << CGameEngine::sm_uiFragmentShaderID << "\n";

	CGameEngine::sm_uiProgramID = glCreateProgram();

	glAttachShader(CGameEngine::sm_uiProgramID, CGameEngine::sm_uiVertexShaderID);
	glAttachShader(CGameEngine::sm_uiProgramID, CGameEngine::sm_uiFragmentShaderID);

	glLinkProgram(CGameEngine::sm_uiProgramID);
	glUseProgram(CGameEngine::sm_uiProgramID);
#pragma endregion

	//Setting partitioning grid
	CGameEngine::sm_colcaCollisionCalculator.m_vec3UpperLeftPoint = { -50,0,-50 };

	//loop and activate each objects ProgramStart
	for (auto item : CGameEngine::sm_rvpgoBuffers[1]) {
		item->ProgramStart();
	}

	//make copies after the start function is run so that any changes done it will transfer
	for (auto item : CGameEngine::sm_rvpgoBuffers[1])
	{
		CGameObject* pgoCopy1 = item->Clone();
		CGameEngine::sm_rvpgoBuffers[0].emplace_back(pgoCopy1);		
	}

	
	for (size_t i = 0; i < CGameEngine::sm_kuiBufferAmount; i++)
	{
		//slight memory optimization
		CGameEngine::sm_rvpgoBuffers[i].shrink_to_fit();

		//register all the tags
		for (CGameObject* pgoItem: this->sm_rvpgoBuffers[i])
		{
			CTagSystem::RegisterHeldTags(pgoItem, i);
		}
	}

	//more OpenGL sends binds
	CGameEngine::sm_uiProjectionMatrixID = glGetUniformLocation(CGameEngine::sm_uiProgramID, "mat4Projection");

	glUniformMatrix4fv(CGameEngine::sm_uiProjectionMatrixID, 1, GL_FALSE, value_ptr(CGameEngine::sm_mat4ProjectionMatrix));

	CGameEngine::sm_uiViewID = glGetUniformLocation(CGameEngine::sm_uiProgramID, "mat4View");
	CGameEngine::sm_uiModelID = glGetUniformLocation(CGameEngine::sm_uiProgramID, "mat4Model");

	//threading
	CGameEngine::sm_uiReadingBuffer = 0;
	CTagSystem::sm_puiCurrentBuffer = &CGameEngine::sm_uiReadingBuffer;

	CGameEngine::sm_uiWritingBuffer = 1;


	CGameEngine::BeginGameLoop();
}

void CGameEngine::DrawScene()
{
	if (CGameEngine::sm_bDebugMode) {
		
		std::cout << "				Drawing scene\n";
		std::cout << "Time spend on last frame: " << CGameEngine::sm_fDeltaTime << " seconds\n";
		std::cout << "Expected framerate: " << 1 / CGameEngine::sm_fDeltaTime << " fps\n";
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region Camera stuff
	CGameEngine::sm_mat4View = mat4(1.0f); //identity

	
	CCamera * pcamMainCamera = static_cast<CCamera*>(CTagSystem::GetTagged("Camera")[0]);

	if (pcamMainCamera == nullptr) {
		std::cout << "Main Camera could not be found\n";
	}

	else
	{
		CCamera::funcpCalculateCamera(*(pcamMainCamera), *CTagSystem::GetTagged("Player")[0]);
		CGameEngine::sm_mat4View = glm::lookAt(pcamMainCamera->m_vec3Position, pcamMainCamera->m_vec3Center, pcamMainCamera->m_vec3Up);

		CRotattingBody* prbCurrent = static_cast<CRotattingBody*>(pcamMainCamera->m_pbdPhysicalBody);

		glUniformMatrix4fv(CGameEngine::sm_uiViewID, 1, GL_FALSE, value_ptr(CGameEngine::sm_mat4View));

		//for specular lights
		vec3 vec3CameraPosition = pcamMainCamera->m_vec3Position;
		glUniform3fv(CGameEngine::sm_uiProgramID, 1, value_ptr(vec3CameraPosition));
	}
#pragma endregion

#pragma region Calling Draw
	//draw every element

	for (auto item : CGameEngine::sm_rvpgoBuffers[CGameEngine::sm_uiReadingBuffer]) {

		glm::mat4 mat4Model = mat4(1);

		mat4Model = glm::translate(mat4Model, item->m_vec3Position);

		CRotattingBody* prbCurrent = static_cast<CRotattingBody*> (item->m_pbdPhysicalBody);
		if (prbCurrent != nullptr) {
			mat4Model = glm::rotate(mat4Model, glm::radians(prbCurrent->m_vec3Orientation.y), vec3(0, 1, 0));
			mat4Model = glm::rotate(mat4Model, glm::radians(prbCurrent->m_vec3Orientation.x), vec3(1, 0, 0));
			mat4Model = glm::rotate(mat4Model, glm::radians(prbCurrent->m_vec3Orientation.z), vec3(0, 0, 1));
		}

		mat4Model = glm::scale(mat4Model, item->m_vec3Scale);

		glUniformMatrix4fv(CGameEngine::sm_uiModelID, 1, GL_FALSE, value_ptr(mat4Model));

		unsigned int uiProgramID = CGameEngine::sm_uiProgramID;
		glUniform1ui(glGetUniformLocation(uiProgramID, "uiVisualizationType"), (int)item->m_evtRenderMode);

		if (item->m_evtRenderMode == CGameObject::EVisualizationType::MATERIALS || item->m_evtRenderMode == CGameObject::EVisualizationType::COMBINED) {
			glUniform3fv(glGetUniformLocation(uiProgramID, "materialObject.m_vec3Ambient"), 1, &(item->m_mtrMaterial.m_vec3Ambient[0]));
			glUniform3fv(glGetUniformLocation(uiProgramID, "materialObject.m_vec3Diffuse"), 1, &(item->m_mtrMaterial.m_vec3Diffuse[0]));
			glUniform3fv(glGetUniformLocation(uiProgramID, "materialObject.m_vec3Specular"), 1, &(item->m_mtrMaterial.m_vec3Specular[0]));
			glUniform3fv(glGetUniformLocation(uiProgramID, "materialObject.m_vec3Emission"), 1, &(item->m_mtrMaterial.m_vec3Emission[0]));
			glUniform3fv(glGetUniformLocation(uiProgramID, "materialObject.m_fGloss"), 1, &(item->m_mtrMaterial.m_fGloss));
		}

		item->Draw();
	}
#pragma endregion

	//std::cout << "Drawing DONE" << "\n";

	if (CInput::m_mKeys['n']) {
		glEnable(GL_MULTISAMPLE);
	}

	else if (CInput::m_mKeys['m']) {
		glDisable(GL_MULTISAMPLE);
	}

	glutSwapBuffers();
}

void CGameEngine::UpdateGame()
{
}

void CGameEngine::CleanupGame()
{
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Cleaning:\n";

	sm_thrPhysics.join();
	//sm_thrUpdate.join();
	sm_thrCollisions.join();
	sm_thrSystemUpdates.join();

	for (size_t i = 0; i < CGameEngine::sm_kuiBufferAmount; i++)
	{
		if (CGameEngine::sm_bDebugMode)
			std::cout << "Buffer " << i +1<< ":\n";

		for (auto item : CGameEngine::sm_rvpgoBuffers[i]) {

			if (CGameEngine::sm_bDebugMode)
				std::cout << item->GetName() << "- ";

			delete item;

			if (CGameEngine::sm_bDebugMode)
				std::cout << "cleaned\n";
		}
	}
}

void CGameEngine::BeginGameLoop()
{
	std::chrono::steady_clock::time_point tmpntSystemStart = std::chrono::steady_clock::now();
	CTime tmTime(tmpntSystemStart);

	//sm_thrUpdate = std::thread(CGameEngine::CalculateUpdate);
	sm_thrPhysics = std::thread(CGameEngine::CalculatePhysics);
	sm_thrCollisions = std::thread(CGameEngine::CalculateCollision);
	sm_thrSystemUpdates = std::thread(CGameEngine::SystemManagement);

	float fPassedTime = 0;

	//90 Hz
	const float kfFrameCap = 0.0111111111111111;

	while (CGameEngine::sm_bContinueThreads)
	{
		if (fPassedTime<kfFrameCap)
		{
			std::chrono::steady_clock::time_point tmpntSystemUpdate = std::chrono::steady_clock::now();
			fPassedTime += tmTime.RecalculateDelta(tmpntSystemUpdate);
			continue;
		}

		CGameEngine::sm_fDeltaTime = fPassedTime;
		fPassedTime = 0;
		glutPostRedisplay();
		CGameEngine::SystemManagement();
		glutMainLoopEvent();
		CGameEngine::UpdateDataBuffers();
	}
}

void CGameEngine::ResizeWindow(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	CGameEngine::sm_mat4ProjectionMatrix = glm::perspective(45.0f, (float)(iWidth / iHeight), 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
}

void CGameEngine::SystemManagement()
{
	if (CInput::m_mKeys[27]) {
		if (CGameEngine::sm_bDebugMode)
			std::cout << "Exiting the program\n";
		CGameEngine::sm_bContinueThreads = false;
		exit(0);
		//glutLeaveMainLoop(); //so the callback fuction for cleanup will be called
	}

	//debugging mode toggle
	if (CInput::m_mSpecialKeys[3]) {
		CGameEngine::sm_bDebugMode = true;
	}
	else if (CInput::m_mSpecialKeys[4]) {
		CGameEngine::sm_bDebugMode = false;
	}

	//check input to change the integration type
	if (CInput::m_mSpecialKeys[9]) {
		if (CGameEngine::sm_bDebugMode)
			std::cout << "Physics Intergration type set to Explicit Eulers\n";

		if (CGameEngine::sm_phcaPhysicsCalculator.CurrentIntegration != CPhysicsCalculator::EIntegrationType::ExplicitEulers)
			CGameEngine::sm_phcaPhysicsCalculator.ChangeCurrentIntegration(CPhysicsCalculator::EIntegrationType::ExplicitEulers);
	}

	else if (CInput::m_mSpecialKeys[10]) {
		if (CGameEngine::sm_bDebugMode)
			std::cout << "Physics Intergration type set to Semi Explicit Eulers\n";

		if (CGameEngine::sm_phcaPhysicsCalculator.CurrentIntegration != CPhysicsCalculator::EIntegrationType::SemiExplicitEulers)
			CGameEngine::sm_phcaPhysicsCalculator.ChangeCurrentIntegration(CPhysicsCalculator::EIntegrationType::SemiExplicitEulers);
	}

	else if (CInput::m_mSpecialKeys[11]) {
		if (CGameEngine::sm_bDebugMode)
			std::cout << "Physics Intergration type set to Velocity Verlet\n";

		if (CGameEngine::sm_phcaPhysicsCalculator.CurrentIntegration != CPhysicsCalculator::EIntegrationType::VelocityVerlet)
			CGameEngine::sm_phcaPhysicsCalculator.ChangeCurrentIntegration(CPhysicsCalculator::EIntegrationType::VelocityVerlet);
	}

	if (CInput::m_mKeys['t'])
	{
		sm_colcaCollisionCalculator.PrintOutBins();
	}
}

#pragma region Input Functions

void CGameEngine::KeyDown(unsigned char ucKey, int iX, int iY) {
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Pressed button code: " << ucKey << "\n";
	CInput::m_mKeys[ucKey] = true;
}

void CGameEngine::KeyUp(unsigned char ucKey, int iX, int iY) {
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Released button code: " << ucKey << "\n";

	CInput::m_mKeys[ucKey] = false;
}

void CGameEngine::KeySpecialDown(int iKeyCode, int x, int y) {
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Pressed button code: " << iKeyCode << "\n";
	CInput::m_mSpecialKeys[iKeyCode] = true;
}

void CGameEngine::KeySpecialUp(int iKeyCode, int x, int y) {
	if (CGameEngine::sm_bDebugMode)
		std::cout << "Released button code: " << iKeyCode << "\n";
	CInput::m_mSpecialKeys[iKeyCode] = false;
}
#pragma endregion

#pragma region Multithreading
void CGameEngine::CalculatePhysics()
{
	std::chrono::steady_clock::time_point tmpntSystemStart = std::chrono::steady_clock::now();
	CTime tmTime(tmpntSystemStart);

	float fPassedTime = 0;

	//180 Hz
	const float kfFrameCap = 0.0055555555555556f;

	while (CGameEngine::sm_bContinueThreads)
	{
		if (fPassedTime < kfFrameCap)
		{
			std::chrono::steady_clock::time_point tmpntSystemUpdate = std::chrono::steady_clock::now();
			fPassedTime += tmTime.RecalculateDelta(tmpntSystemUpdate);
			continue;
		}


		if (CGameEngine::sm_bDebugMode)
			std::cout << "			Update\n";

		//run through game object updates which calculates all the changes

		CGameObject::sm_fDeltaTime = fPassedTime;

		for (auto item : CGameEngine::sm_rvpgoBuffers[CGameEngine::sm_uiWritingBuffer]) {
			item->ProgramUpdate();
		}


		if (CGameEngine::sm_bDebugMode)
			std::cout << "	Adding gravity\n";

		for (auto item : CGameEngine::sm_rvpgoBuffers[CGameEngine::sm_uiWritingBuffer])
		{
			if (item->m_pbdPhysicalBody != nullptr && item->m_pbdPhysicalBody->m_bIsAffectedByGravity) {
				vec3 gravityForce(0, CPhysicsCalculator::m_ksfGravity, 0);
				item->m_pbdPhysicalBody->m_vec3TotalLineraForce += gravityForce * item->m_pbdPhysicalBody->m_fMass;
			}
		}

		//std::cout << "	Calculating physics done" << "\n";

		//run through physics calculations
		if (CGameEngine::sm_bDebugMode)
			std::cout << "	Calculating physics\n";

		//std::cout << fDeltaTime << "\n";
		sm_phcaPhysicsCalculator.RecalculatePhysics(CGameEngine::sm_rvpgoBuffers[CGameEngine::sm_uiWritingBuffer], CGameEngine::sm_rvpgoBuffers[CGameEngine::sm_uiReadingBuffer], fPassedTime);
		fPassedTime = 0;
	}
}

void CGameEngine::CalculateCollision()
{
	std::chrono::steady_clock::time_point tmpntSystemStart = std::chrono::steady_clock::now();
	CTime tmTime(tmpntSystemStart);

	float fPassedTime = 0;

	//180 Hz
	const float kfFrameCap = 0.0055555555555556f;

	while (CGameEngine::sm_bContinueThreads)
	{
		if (fPassedTime < kfFrameCap)
		{
			std::chrono::steady_clock::time_point tmpntSystemUpdate = std::chrono::steady_clock::now();
			fPassedTime += tmTime.RecalculateDelta(tmpntSystemUpdate);
			continue;
		}

		fPassedTime = 0;

		if (CGameEngine::sm_bDebugMode)
			std::cout << "		Checking collisions\n";

		vector<CGameObject*> vpgoReadable = CTagSystem::GetTagged("Collides");
		vector<CGameObject*> vpgoWriteable = CTagSystem::GetTaggedSpecific("Collides", CGameEngine::sm_uiWritingBuffer);

		CGameEngine::sm_colcaCollisionCalculator.RecalculateCollision(vpgoWriteable, vpgoReadable);
	}
}

void CGameEngine::CalculateUpdate()
{	
	std::chrono::steady_clock::time_point tmpntSystemStart = std::chrono::steady_clock::now();
	CTime tmTime(tmpntSystemStart);

	float fPassedTime = 0;

	//180 Hz
	const float kfFrameCap = 0.0055555555555556f;

	while (CGameEngine::sm_bContinueThreads)
	{
		if (fPassedTime < kfFrameCap)
		{
			std::chrono::steady_clock::time_point tmpntSystemUpdate = std::chrono::steady_clock::now();
			fPassedTime += tmTime.RecalculateDelta(tmpntSystemUpdate);
			continue;
		}

		

		//std::cout << "		Update DONE" << "\n";
		fPassedTime = 0;
	}
	
}

void CGameEngine::UpdateDataBuffers()
{
	CGameEngine::sm_uiReadingBuffer = CGameEngine::sm_uiWritingBuffer;

	CGameEngine::sm_uiWritingBuffer++;

	if (CGameEngine::sm_uiWritingBuffer >= CGameEngine::sm_kuiBufferAmount)
		CGameEngine::sm_uiWritingBuffer = 0;
	std::cout << "SWITCH\n";
}

#pragma endregion