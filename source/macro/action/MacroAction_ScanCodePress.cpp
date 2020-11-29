
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroAction_ScanCodePress.h"
#include "win.h"

MacroAction_ScanCodePress::MacroAction_ScanCodePress(int scanCode, bool hasPrefixE0)
	: MacroAction()
{
	m_scanCode		= scanCode;
	m_hasPrefixE0	= hasPrefixE0;
	m_isKeyDown		= true;
	m_isKeyUp		= true;
}

bool	MacroAction_ScanCodePress::start()
{
	INPUT input;
	input.type				= INPUT_KEYBOARD;
    input.ki.wVk			= 0;
    input.ki.wScan			= m_scanCode;
    input.ki.dwFlags		= KEYEVENTF_SCANCODE | (m_hasPrefixE0 ? KEYEVENTF_EXTENDEDKEY : 0);
    input.ki.time			= 0;
    input.ki.dwExtraInfo	= 0;

	// key down
	if (m_isKeyDown)
		SendInput(1, &input, sizeof(input));

	// key up
    input.ki.dwFlags		|= KEYEVENTF_KEYUP;
	if (m_isKeyUp)
		SendInput(1, &input, sizeof(input));

	return true;
}
