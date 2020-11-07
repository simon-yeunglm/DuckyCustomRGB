#pragma once

// by simon yeung, 10/10/2020
// all rights reserved

#include "Keyboard.h"

class Keyboard_One2RGB_TKL : public Keyboard
{
public:
	static unsigned short productID;

	Keyboard_One2RGB_TKL(HIDDevice* device);
	virtual ~Keyboard_One2RGB_TKL();
	
	virtual void	connect()override;
	virtual void	disconnect()override;
};
