
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroAction_KeyPress.h"
#include "win.h"
#include "keyboard/KeyboardKey.h"

MacroAction_KeyPress::MacroAction_KeyPress(KeyboardKey key)
	: MacroAction()
{
	m_virtualKeyCode= KeyboardKeyToVirtualKeyCode(key);
	m_isKeyDown	= true;
	m_isKeyUp	= true;
}

bool	MacroAction_KeyPress::start()
{
	INPUT input;
	input.type				= INPUT_KEYBOARD;
    input.ki.wVk			= (WORD)m_virtualKeyCode;
    input.ki.wScan			= 0;
    input.ki.dwFlags		= 0;
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
