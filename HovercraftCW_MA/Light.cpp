#include "Light.h"
#include <glm\gtx\string_cast.hpp>
vec3 CLight::sm_vec3GlobalAmbience;

void CLight::ProgramStart()
{
    this->m_vec3AmbientColour = {0.6, 0.6f, 0.6f};
    this->m_vec3DiffuseColour = {0.7f, 0.7f, 0.7f};
    this->m_vec3SpecularColour = {0.8f, 0.8f, 0.8f};
    this->m_vec3Direction = { 1 , 1, 0};

    CLight::sm_vec3GlobalAmbience = {0.15f, 0.15f, 0.15f};

    unsigned int uiProgramID = *(CGameObject::sm_puiProgramID);
    glUniform3fv(glGetUniformLocation(uiProgramID, "vec3GlobalAmbience"), 1, &(CLight::sm_vec3GlobalAmbience[0]));

    glUniform3fv(glGetUniformLocation(uiProgramID, "directionalLight.m_vec3Ambient"), 1, &(this->m_vec3AmbientColour[0]));
    glUniform3fv(glGetUniformLocation(uiProgramID, "directionalLight.m_vec3Diffuse"), 1, &(this->m_vec3DiffuseColour[0]));
    glUniform3fv(glGetUniformLocation(uiProgramID, "directionalLight.m_vec3Specular"), 1, &(this->m_vec3SpecularColour[0]));
    glUniform3fv(glGetUniformLocation(uiProgramID, "directionalLight.m_vec3Direct"), 1, &(this->m_vec3Direction[0]));

    this->m_tsTag.m_vstrHeldTags.emplace_back("Main light");
}

void CLight::ProgramUpdate()
{
}

CGameObject* CLight::Clone()
{
    CLight* plgtClone = new CLight(*this);
    return plgtClone;
}

void CLight::Draw() {
    //because gl calls have to be inside glutMainLoop/Event and updates are called separately so it has to be inside draw
    //check for input and the rotate

    if (CInput::m_mKeys['.']) {

        CLight* plghReadable = static_cast<CLight*>(CTagSystem::GetTagged("Main light")[0]);
        this->m_vec3Direction.x = plghReadable->m_vec3Direction.x - 10 * CGameObject::sm_fDeltaTime;
        glUniform3fv(glGetUniformLocation(*(CGameObject::sm_puiProgramID), "directionalLight.m_vec3Direct"), 1, &(plghReadable->m_vec3Direction[0]));
    }

    else if (CInput::m_mKeys[',']) {
        CLight* plghReadable = static_cast<CLight*>(CTagSystem::GetTagged("Main light")[0]);
        this->m_vec3Direction.x = plghReadable->m_vec3Direction.x + 10 * CGameObject::sm_fDeltaTime;
        glUniform3fv(glGetUniformLocation(*(CGameObject::sm_puiProgramID), "directionalLight.m_vec3Direct"), 1, &(plghReadable->m_vec3Direction[0]));
    }

    //send out light type
    if (CInput::m_mKeys[';']) {
        glUniform1ui(glGetUniformLocation(*(CGameObject::sm_puiProgramID), "uiLighType"), (int)CLight::ELightType::DIRECTIONAL);
    }

    else if (CInput::m_mKeys['\'']) {
        glUniform1ui(glGetUniformLocation(*(CGameObject::sm_puiProgramID), "uiLighType"), (int)CLight::ELightType::POINT_OF_LIGHT);
    }
}
