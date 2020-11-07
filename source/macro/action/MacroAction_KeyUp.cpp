
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroAction_KeyUp.h"

MacroAction_KeyUp::MacroAction_KeyUp(KeyboardKey key)
	: MacroAction_KeyPress(key)
{
	m_isKeyDown	= false;
	m_isKeyUp	= true;
}
