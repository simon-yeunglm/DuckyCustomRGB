#pragma once

// by simon yeung, 26/10/2020
// all rights reserved

#include "AudioVolumeCallback.h"

class Audio
{
public:
	static void init();
	static void relrease();
	static void registerVolumeCallback(		AudioVolumeCallback callback, void* userPtr);
	static void deregisterVolumeCallback(	AudioVolumeCallback callback, void* userPtr);
};
