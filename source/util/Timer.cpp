
// by simon yeung, 18/10/2020
// all rights reserved

#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency(&m_freq);
	restart();
}

void	Timer::restart()
{
	m_totalTime		= 0;
	m_elapsedTime	= 0;
	QueryPerformanceCounter(&m_startTime);
}

float	Timer::tick()
{
	LARGE_INTEGER	endTime;
	QueryPerformanceCounter(&endTime);
	double elapsedTime= ((double)(endTime.QuadPart-m_startTime.QuadPart))/(double)m_freq.QuadPart;
	m_totalTime		+= elapsedTime;
	m_elapsedTime	= (float)elapsedTime;
	m_startTime= endTime;
	return getElapsedTime();
}
