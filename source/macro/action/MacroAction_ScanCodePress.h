#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

#include "macro/MacroAction.h"

enum class KeyboardKey;

class MacroAction_ScanCodePress : public MacroAction
{
protected:
	int		m_scanCode;
	bool	m_hasPrefixE0;
	bool	m_isKeyDown;
	bool	m_isKeyUp;

public:
	MacroAction_ScanCodePress(int scanCode, bool hasPrefixE0);
	virtual bool	start();
};

