#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

#include "macro/MacroAction.h"

enum class KeyboardKey;

class MacroAction_KeyPress : public MacroAction
{
protected:
	unsigned char	m_virtualKeyCode;
	bool			m_isKeyDown;
	bool			m_isKeyUp;

public:
	MacroAction_KeyPress(KeyboardKey key);
	virtual bool	start();
};

