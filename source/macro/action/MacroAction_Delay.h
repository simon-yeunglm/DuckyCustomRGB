#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

#include "macro/MacroAction.h"

class MacroAction_Delay : public MacroAction
{
protected:
	float	m_delayTime;
	float	m_timer;

public:
	MacroAction_Delay(float delayTime);

	virtual bool	start();
	virtual bool	update(float deltaSeconds);
};

