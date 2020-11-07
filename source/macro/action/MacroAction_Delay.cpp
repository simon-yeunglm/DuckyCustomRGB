
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroAction_Delay.h"

MacroAction_Delay::MacroAction_Delay(float delayTime)
	: MacroAction()
{
	m_delayTime = delayTime;
	m_timer		= 0;
}

bool	MacroAction_Delay::start()
{
	m_timer		= 0;
	return false;
}

bool	MacroAction_Delay::update(float deltaSeconds)
{
	m_timer+= deltaSeconds;
	return m_timer >= m_delayTime;
}
