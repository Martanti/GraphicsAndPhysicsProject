#include "GameObject.h"
#include "RotattingBody.h"

unsigned int* CGameObject::sm_puiProgramID;
bool* CGameObject::sm_pbDebugMode;
unsigned int CGameObject::sm_uiTexturedObjects = 0;
float CGameObject::sm_fDeltaTime;

CGameObject::CGameObject(string* pstrPathToModel, string* pstrPathToTexture, SMaterial* pmatrMaterial)
{
	this->m_strName = "General game object";

	glGenVertexArrays(1, &m_uiVertexArrayObject);
	glGenBuffers(1, &m_uiVertexBufferObject);

	if (pstrPathToModel != nullptr)
		this->LoadModel(pstrPathToModel);

	if (pstrPathToTexture != nullptr)
		this->LoadTexture(pstrPathToTexture);

	if (pmatrMaterial != nullptr)
		this->LoadMaterial(pmatrMaterial);

#pragma region Render mode determination
	if (pstrPathToTexture != nullptr && pmatrMaterial != nullptr)
		this->m_evtRenderMode = CGameObject::EVisualizationType::COMBINED;

	else if (pstrPathToTexture != nullptr)
		this->m_evtRenderMode = CGameObject::EVisualizationType::TEXTURED;

	else if (pmatrMaterial != nullptr)
		this->m_evtRenderMode = CGameObject::EVisualizationType::MATERIALS;

	else
		this->m_evtRenderMode = CGameObject::EVisualizationType::NO_DATA;
#pragma endregion
}

CGameObject::~CGameObject(){
	if (this->m_pbdPhysicalBody != nullptr) {
		delete this->m_pbdPhysicalBody;
	}
}

void CGameObject::ProgramStart(){
}

void CGameObject::ProgramUpdate(){
}

CGameObject* CGameObject::Clone()
{
	CGameObject* pgoClone = new CGameObject(*this);

	if (this->m_pbdPhysicalBody != nullptr)
	{
		CRotattingBody* prbCurrent = static_cast<CRotattingBody*>(this->m_pbdPhysicalBody);
		if (prbCurrent != nullptr)
		{
			CRotattingBody* prbClone = new CRotattingBody(*prbCurrent);
			pgoClone->m_pbdPhysicalBody = prbClone;
		}
		else
		{
			CPhysicalBody* ppbClone = new CPhysicalBody(*this->m_pbdPhysicalBody);

			pgoClone->m_pbdPhysicalBody = ppbClone;
		}
	}
	return pgoClone;
}

void CGameObject::Draw(){

	glBindVertexArray(this->m_uiVertexArrayObject);

	if (this->m_pobjrGeometry != nullptr)
	{
		unsigned int uiTextureLocation = glGetUniformLocation(*CGameObject::sm_puiProgramID, "Texture2D");
		glUniform1i(uiTextureLocation, this->m_uiTexturePositionOffset);


		glDrawArrays(GL_TRIANGLES, 0, this->m_pobjrGeometry->numVertices);
	}

	if (*CGameObject::sm_pbDebugMode && this->m_pcolCollider != nullptr)
	{
		glUniform1ui(glGetUniformLocation(*CGameObject::sm_puiProgramID, "uiVisualizationType"), (int)CGameObject::EVisualizationType::NO_DATA);
		this->m_pcolCollider->DrawShape();
	}
}

void CGameObject::LoadModel(string* pstrPathToModel)
{
	this->m_pobjrGeometry = std::make_shared<objReader>();
	this->m_pobjrGeometry->LoadModel((char *) pstrPathToModel->c_str());

	glBindVertexArray(this->m_uiVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiVertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, sizeof(fVertex) * this->m_pobjrGeometry->numVertices, this->m_pobjrGeometry->v, GL_STREAM_DRAW);

	//coorindates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(fVertex), 0);
	glEnableVertexAttribArray(0);

	//texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(fVertex), (void*)(sizeof(fVertex::coords)));
	glEnableVertexAttribArray(1);

	//normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(fVertex), (void*)(sizeof(fVertex::texCoords) + sizeof(fVertex::coords)));
	glEnableVertexAttribArray(2);
}

void CGameObject::LoadTexture(string* pstrPathToTexture)
{
	CGameObject::sm_uiTexturedObjects++;
	this->m_uiTexturePositionOffset = CGameObject::sm_uiTexturedObjects;


	glGenTextures(1, &this->m_uiTextureID);

	glActiveTexture(GL_TEXTURE0 + this->m_uiTexturePositionOffset);

	glBindTexture(GL_TEXTURE_2D, this->m_uiTextureID);


	int iTextureWidth, iTextureHeight, iTextureChannels;

	unsigned char* pucImagaData = stbi_load(pstrPathToTexture->c_str(), &iTextureWidth, &iTextureHeight, &iTextureChannels, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (pucImagaData) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iTextureWidth, iTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pucImagaData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else
		std::cout << "Couldn't load texture from " << *pstrPathToTexture << "\n";

	stbi_image_free(pucImagaData);
}

void CGameObject::LoadMaterial(SMaterial* pmatrMaterial)
{
	this->m_mtrMaterial = *pmatrMaterial;
}
