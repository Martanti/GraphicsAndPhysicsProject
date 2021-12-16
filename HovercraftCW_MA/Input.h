#include <map>

#pragma once

using std::map;

class CInput
{
public:
	CInput();
	~CInput();

	static map<char, bool> m_mKeys;
	static map<int, bool> m_mSpecialKeys;
};

