
// by simon yeung, 15/12/2021
// all rights reserved

#include "RenderTarget.h"

bool	RenderTarget::getAverageColorInRange(int4 pxRange, float* outR, float* outG, float* outB)
{
	int		numPx		= (pxRange.z - pxRange.x) * (pxRange.w - pxRange.y);
	if (numPx <= 0)
		return false;

	float3	averageVal	= { 0, 0, 0 };
	for(int x= pxRange.x; x<pxRange.z; ++x)
		for(int y= pxRange.y; y<pxRange.w; ++y)
		{
			float4 pxColor= getPixel(x, y);
			averageVal+= pxColor.xyz();
		}
	averageVal/= (float)numPx;
		
	*outR= averageVal.x;
	*outG= averageVal.y;
	*outB= averageVal.z;
	return true;
}

bool	RenderTarget::getAverageColorInRange(int4 pxRange, unsigned char* outR, unsigned char* outG, unsigned char* outB)
{
	float Rf, Gf, Bf;
	if (getAverageColorInRange(pxRange, &Rf, &Gf, &Bf))
	{
		*outR= (unsigned char)(Rf * 255);
		*outG= (unsigned char)(Gf * 255);
		*outB= (unsigned char)(Bf * 255);
		return true;
	}
	else
		return false;
}
