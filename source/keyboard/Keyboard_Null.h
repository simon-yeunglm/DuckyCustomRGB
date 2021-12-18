#pragma once

// by simon yeung, 05/10/2021
// all rights reserved

#include "Keyboard.h"

class Keyboard_Null : public Keyboard
{
public:
	Keyboard_Null();
	virtual ~Keyboard_Null();
	
	virtual void commitKeyColor(RenderTarget* renderTarget)override;
	
	virtual void	connect()override;
	virtual void	disconnect()override;
};
