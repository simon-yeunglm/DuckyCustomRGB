
// by simon yeung, 01/11/2020
// all rights reserved

#include "MacroAction_VolumeUp.h"
#include "win.h"
#include "keyboard/KeyboardKey.h"

MacroAction_VolumeUp::MacroAction_VolumeUp()
	: MacroAction_KeyPress(KeyboardKey::A)
{
	m_virtualKeyCode= VK_VOLUME_UP;
}