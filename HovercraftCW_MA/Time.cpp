#include "Time.h"

CTime::CTime(std::chrono::steady_clock::time_point& rtmpntNow)
{
	this->m_tmpntOldTime = rtmpntNow;
}

CTime::~CTime()
{
}


float CTime::RecalculateDelta(std::chrono::steady_clock::time_point& rtmpntNow)
{
	float fDelta = std::chrono::duration_cast<std::chrono::nanoseconds>(rtmpntNow - this->m_tmpntOldTime).count() * 0.000000001;
	this->m_tmpntOldTime = rtmpntNow;
	return fDelta;
}