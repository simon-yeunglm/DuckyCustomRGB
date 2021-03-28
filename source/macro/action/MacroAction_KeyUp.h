#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroAction_KeyPress.h"

class MacroAction_KeyUp : public MacroAction_KeyPress
{
public:
	MacroAction_KeyUp(InputKey key);
};
