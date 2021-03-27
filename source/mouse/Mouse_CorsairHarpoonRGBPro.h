#pragma once

// by simon yeung, 27/03/2021
// all rights reserved

#include "Mouse.h"

class Mouse_CorsairHarpoonRGBPro : public Mouse
{
public:
	static unsigned short productID;

	Mouse_CorsairHarpoonRGBPro(HIDDevice* device);
	virtual ~Mouse_CorsairHarpoonRGBPro();
	
	virtual void	connect()override;
	virtual void	disconnect()override;

	virtual void	setColor(float R, float G, float B)override;
};
