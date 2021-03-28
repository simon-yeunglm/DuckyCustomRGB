
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroAction_KeyDown.h"

MacroAction_KeyDown::MacroAction_KeyDown(InputKey key)
	: MacroAction_KeyPress(key)
{
	m_isKeyDown	= true;
	m_isKeyUp	= false;
}
