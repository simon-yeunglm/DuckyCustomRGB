
// by simon yeung, 18/10/2020
// all rights reserved

#include "Color.h"
#include "math\Math.h"
#include "math\vector.h"

// conversion from http://www.chilliant.com/rgb2hsv.html
float3 Color::HUEtoRGB(float H)
{
	float R = Math::abs(H * 6 - 3) - 1;
	float G = 2 - Math::abs(H * 6 - 2);
	float B = 2 - Math::abs(H * 6 - 4);
	float3 val= {R, G, B };
	return saturate(val);
}

float3 Color::RGBtoHCV(float3 RGB)
{
	float Epsilon = 1e-10f;
	// Based on work by Sam Hocevar and Emil Persson
	float4 P = (RGB.y < RGB.z) ? float4::create(RGB.z, RGB.y, -1.0f, 2.0f/3.0f) : float4::create(RGB.y, RGB.z, 0.0, -1.0f/3.0f);
	float4 Q = (RGB.x < P.x) ? float4::create(P.x, P.y, P.w, RGB.x) : float4::create(RGB.x, P.y, P.z, P.x);
	float C = Q.x - Math::min(Q.w, Q.y);
	float H = Math::abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
	return { H, C, Q.x };
}

float3 Color::HSVtoRGB(float3 HSV)
{
	float3 RGB = HUEtoRGB(HSV.x);
	return ((RGB - 1) * HSV.y + 1) * HSV.z;
}

float3 Color::RGBtoHSV(float3 RGB)
{
	float Epsilon = 1e-10f;
	float3 HCV = RGBtoHCV(RGB);
	float S = HCV.y / (HCV.z + Epsilon);
	return { HCV.x, S, HCV.z };
}
