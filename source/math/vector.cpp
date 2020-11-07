
// by simon yeung, 18/10/2020
// all rights reserved

#include "vector.h"
#include "Math.h"

float float2::length()const
{
	return Math::sqrt(x*x+y*y);
}

float3 saturate(const float3& v)
{
	float3 newV=
	{
		Math::clamp(v.x, 0.0f, 1.0f),
		Math::clamp(v.y, 0.0f, 1.0f),
		Math::clamp(v.z, 0.0f, 1.0f),
	};
	return newV;
}
