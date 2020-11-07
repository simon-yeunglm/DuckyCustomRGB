#pragma once

// by simon yeung, 18/10/2020
// all rights reserved

struct float3;

class Color
{
public:
	static float3	HUEtoRGB(float H);
	static float3	RGBtoHCV(float3 RGB);
	static float3	HSVtoRGB(float3 HSV);
	static float3	RGBtoHSV(float3 RGB);
};
