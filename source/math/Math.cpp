
// by simon yeung, 18/10/2020
// all rights reserved

#include "Math.h"
#include <math.h>

float	Math::abs(float val)
{
	return fabsf(val);
}

float			Math::mod(float dividend, float divisor)
{
	return fmodf(dividend, divisor);
}

float		Math::floor(float val)
{
	return floorf(val);
}

float		Math::ceil(float val)
{
	return ceilf(val);
}

float		Math::sqrt(float val)
{
	return sqrtf(val);
}

float			Math::round(float val)
{
	return roundf(val);
}
