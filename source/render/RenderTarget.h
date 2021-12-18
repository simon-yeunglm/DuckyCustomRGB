#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

#include "math/vector.h"
#include <assert.h>

// a texture represent color of the keyboard,
// 4x4 pixel == size of 1u key cap

#define NUM_PIXEL_PER_KEY		(4)

#pragma warning( push )
#pragma warning( disable : 4200 )		// for flexible array member
struct RenderTarget
{
public:
	int2	size;

	inline float4	getPixel(int x, int y)					{ return	pixels[getIdx(x, y)];			}
	inline void		setPixel(int x, int y, float4 color)	{			pixels[getIdx(x, y)]= color;	}
	inline void		setPixel(int x, int y, float3 color)	{			pixels[getIdx(x, y)].set(color);}
	
	// return whether pxRange is valid
	bool			getAverageColorInRange(int4 pxRange, float* outR, float* outG, float* outB);
	bool			getAverageColorInRange(int4 pxRange, unsigned char* outR, unsigned char* outG, unsigned char* outB);

private:
	float4	pixels[];	// [size.x * size.y], xyz= RGB, z= A

	// TODO: try Morton order?
	int	getIdx(int x, int y)
	{
		int idx= y * size.x + x;
		assert(idx < size.x*size.y);
		return idx;
	}
};
#pragma warning( pop )
