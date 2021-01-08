
// by simon yeung, 09/01/2021
// all rights reserved

#include "MacroAction_MediaPlayPause.h"
#include "win.h"
#include "keyboard/KeyboardKey.h"

MacroAction_MediaPlayPause::MacroAction_MediaPlayPause()
	: MacroAction_KeyPress(KeyboardKey::A)
{
	m_virtualKeyCode= VK_MEDIA_PLAY_PAUSE;
}