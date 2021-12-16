#include <chrono>

#pragma once
class CTime
{
public:

	CTime() = default;
	CTime(std::chrono::steady_clock::time_point & rtmpntNow);
	~CTime();

	float RecalculateDelta(std::chrono::steady_clock::time_point & rtmpntNow);
private:

	std::chrono::steady_clock::time_point m_tmpntOldTime;
};

