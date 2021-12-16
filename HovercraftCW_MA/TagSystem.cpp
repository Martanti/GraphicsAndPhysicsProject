#include "TagSystem.h"
#include "GameObject.h"

unsigned int* CTagSystem::sm_puiCurrentBuffer;
std::map<std::string, std::vector<CGameObject*>> CTagSystem::sm_rmpTagBuffers[2];



void CTagSystem::RegisterHeldTags(CGameObject* pgoRegistered, unsigned int uiBuffer) {
	for (std::string strHeldTags : pgoRegistered->m_tsTag.m_vstrHeldTags)
	{
		CTagSystem::sm_rmpTagBuffers[uiBuffer][strHeldTags].emplace_back(pgoRegistered);
	}

}


std::vector<CGameObject*> CTagSystem::GetTagged(std::string strTag)
{
	return CTagSystem::sm_rmpTagBuffers[*CTagSystem::sm_puiCurrentBuffer][strTag];
}

std::vector<CGameObject*> CTagSystem::GetTaggedSpecific(std::string strTag, unsigned int uiBuffer)
{
	return CTagSystem::sm_rmpTagBuffers[uiBuffer][strTag];
}

