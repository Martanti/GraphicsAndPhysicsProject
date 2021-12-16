#pragma once

//#include "GameObject.h"
#include <map>
#include <vector>
#include <string>


class CGameObject;

class CTagSystem
{
public:
	std::vector<std::string> m_vstrHeldTags;
	static void RegisterHeldTags(CGameObject* pgoRegistered, unsigned int uiBuffer);
	static std::vector<CGameObject*> GetTagged(std::string strTag);
	static std::vector<CGameObject*> GetTaggedSpecific(std::string strTag, unsigned int uiBuffer);
	static unsigned int* sm_puiCurrentBuffer; 
private:
	static std::map<std::string, std::vector<CGameObject*>> sm_rmpTagBuffers[2];
};

