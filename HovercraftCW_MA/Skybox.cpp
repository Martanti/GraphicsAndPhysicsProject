#include "Skybox.h"
#include <glm\gtx\string_cast.hpp>

CSkybox::CSkybox(): CGameObject() {

}

CSkybox::~CSkybox()
{
}

void CSkybox::ProgramStart()
{
	//load 6 images 
	//and treat every wall as a singluar plane

	this->m_evtRenderMode = CGameObject::EVisualizationType::SKYBOX;

	this->m_vec3Position = { 0,0,0 };

	glBindVertexArray(this->m_uiVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiVertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, this->m_frSkyboxVertices, GL_STREAM_DRAW);

	////coorindates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	//texture
	glGenTextures(1, &this->m_uiTextureID);

	CGameObject::sm_uiTexturedObjects++;
	this->m_uiTexturePositionOffset = CGameObject::sm_uiTexturedObjects;

	glActiveTexture(GL_TEXTURE0 + this->m_uiTexturePositionOffset);

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_uiTextureID);


	int iTextureWidth, iTextureHeight, iTextureChannels;
	string kpcTextureLocation = "../Geometry/elyvisions/elyvisions/";

	int iIndex = 0;
	for (auto item : this->m_strrFileNames)
	{

		string b = kpcTextureLocation +   item;
		const char* a = b.c_str();

		unsigned char* pucImagaData = stbi_load(a , &iTextureWidth, &iTextureHeight, &iTextureChannels, 0);

		if (pucImagaData) {

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+iIndex, 0, GL_RGB, iTextureWidth, iTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pucImagaData);
		}

		else
			std::cout << "Couldn't load texture from " << kpcTextureLocation << "\n";

		stbi_image_free(pucImagaData);

		iIndex++;
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	this->m_vec3Scale = { 60,60,60 };

}

void CSkybox::ProgramUpdate()
{
	CCamera* pcamCamera = static_cast<CCamera*>(CTagSystem::GetTagged("Camera")[0]);
	//std::cout << "	camera pos " << glm::to_string(pcamCamera->m_vec3Position)<<"\n";
	this->m_vec3Position = pcamCamera->m_vec3Position;
}

void CSkybox::Draw()
{
	glDepthMask(GL_FALSE);

	glBindVertexArray(this->m_uiVertexArrayObject);

	unsigned int uiTextureLocation = glGetUniformLocation(*CGameObject::sm_puiProgramID, "skybox");
	glUniform1i(uiTextureLocation, this->m_uiTexturePositionOffset);

	glActiveTexture(GL_TEXTURE0 + this->m_uiTexturePositionOffset);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

CGameObject* CSkybox::Clone()
{
	CSkybox* pskyClone = new CSkybox(*this);
	return pskyClone;
}
