#pragma once

// by simon yeung, 18/10/2020
// all rights reserved

#include "win.h"

class Timer
{
private:
	LARGE_INTEGER	m_freq;
	LARGE_INTEGER	m_startTime;
	double			m_totalTime;
	float			m_elapsedTime;

public:
	Timer();
	~Timer()=default;
	
	void			restart();
	float			tick();

	inline void		begin()					{ restart(); }
	inline double	end()					{ tick(); return getTotalTime(); }

	inline double	getTotalTime()const		{ return m_totalTime; }
	inline float	getElapsedTime()const	{ return m_elapsedTime; }
};

