#pragma once

// by simon yeung, 27/03/2021
// all rights reserved

#include "usb/USBDevice.h"

class HIDDevice;

class Mouse : public USBDevice
{
public:
	Mouse(HIDDevice* device);
	virtual ~Mouse();
	
	// TODO: add enum to specify LED or refactor code to query from render target
	virtual void	setColor(float R, float G, float B)= 0;
};
