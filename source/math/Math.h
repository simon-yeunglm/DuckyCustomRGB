#pragma once

// by simon yeung, 13/10/2020
// all rights reserved

class Math
{
public:
	template<typename type>
	static type			min(type val0, type val1);
	
	template<typename type>
	static type			max(type val0, type val1);
	
	template<typename type, typename typeT>
	static type			lerp(type from, type to, typeT t);

	template<typename type>
	static type			clamp(type val, type min, type max);

	template<typename type>
	static type			smoothStep(type t);
	
	static float		abs(float val);
	static float		mod(float dividend, float divisor);
	static float		floor(float val);
	static float		ceil(float val);
	static float		sqrt(float val);
	static float		round(float val);
};

template<typename type>
type	Math::min(type val0, type val1)
{
    return val0 < val1 ? val0 : val1;
}
	
template<typename type>
type	Math::max(type val0, type val1)
{
    return val0 > val1 ? val0 : val1;
}
	
template<typename type, typename typeT>
type	Math::lerp(type from, type to, typeT t)
{
	return (to - from) * t + from;
}

template<typename type>
type	Math::clamp(type val, type min, type max)
{
    return Math::max(min, Math::min(max, val));
}

template<typename type>
type			Math::smoothStep(type t)
{
    return t*t*(3.0f - 2.0f*t);
}
