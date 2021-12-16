
#pragma region Includes
#include<string>
#include <glm/glm.hpp>
#include "PhysicalBody.h"
#include "Input.h"
#include "TagSystem.h"
#include "Material.h"

#include <iostream>
#include <GL\glew.h>
#include "objReader.h"
#include "Collider.h"
#include "stb_image.h"

#pragma endregion

#pragma once

using std::string;
using glm::vec3;
using glm::mat4;
using glm::mat3;

class CGameObject
{
public:

	enum class EVisualizationType {NO_DATA, TEXTURED, MATERIALS, COMBINED, SKYBOX};

	CGameObject(string* rstrPathToModel = nullptr, string* rstrPathToTexture = nullptr, SMaterial* rmatrMaterial = nullptr);
	
	~CGameObject();

	//Things that need to be done at the start of the program
	virtual void ProgramStart();

	//Things that need to be done every frame
	virtual void ProgramUpdate();

	virtual CGameObject* Clone();

	//Get the name of the game object
	inline string GetName() { return m_strName; }

	CTagSystem m_tsTag;

	//Draw the current object
	virtual void Draw();

	vec3 m_vec3Position;

	vec3 m_vec3Scale = { 1,1,1 };

	CPhysicalBody* m_pbdPhysicalBody;

	std::shared_ptr<CCollider> m_pcolCollider;

	//OpenGL links to the shader

	unsigned int m_uiVertexArrayObject;
	unsigned int m_uiVertexBufferObject;

	unsigned int m_uiTextureID;

	static unsigned int* sm_puiProgramID;
	static bool* sm_pbDebugMode;
	static float sm_fDeltaTime;

	bool m_bIsStatic;

	SMaterial m_mtrMaterial;

	static unsigned int sm_uiTexturedObjects;

	unsigned int m_uiTexturePositionOffset;

	//Used to determine how should shader render this object
	CGameObject::EVisualizationType m_evtRenderMode;

	std::shared_ptr< objReader>  m_pobjrGeometry;
	string m_strName;



private:
	void LoadModel(string* pstrPathToModel);
	void LoadTexture(string* pstrPathToTexture);
	void LoadMaterial(SMaterial* pmatrMaterial);

};

