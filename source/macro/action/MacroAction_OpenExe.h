#pragma once

// by simon yeung, 01/11/2020
// all rights reserved

#include "macro/MacroAction.h"

class MacroAction_OpenExe : public MacroAction
{
protected:
	char*	m_exePath;

public:
	MacroAction_OpenExe(const char* exePath);
	virtual ~MacroAction_OpenExe();

	virtual bool	start();
};

