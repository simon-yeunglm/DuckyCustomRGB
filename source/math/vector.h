#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

struct int2
{
	int	x;
	int y;

	inline void set(int _x, int _y) { x= _x; y= _y; }
};

struct int4
{
	int	x;
	int y;
	int z;
	int w;
	
	inline void set(int _x, int _y, int _z, int _w) { x= _x; y= _y; z= _z; w= _w; }
};

struct float2
{
	float x;
	float y;

	float length()const;
};

float2 operator+ (const float2& lhs, const float2& rhs);
float2 operator- (const float2& lhs, const float2& rhs);
float2 operator* (const float2& lhs, const float2& rhs);
float2 operator/ (const float2& lhs, const float2& rhs);

float2 operator+ (const float2& v, float s);
float2 operator+ (float s, const float2& v);
float2 operator- (const float2& v, float s);
float2 operator- (float s, const float2& v);

struct float3
{
	float x;
	float y;
	float z;
	
	float3			operator- () const;
	float3&			operator+= (const float3& v);
	float3&			operator-= (const float3& v);
	float3&			operator*= (const float3& v);
	float3&			operator/= (const float3& v);
	float3&			operator+= (float s);
	float3&			operator-= (float s);
	float3&			operator*= (float s);
	float3&			operator/= (float s);
};

struct float4
{
	float x;
	float y;
	float z;
	float w;

	static float4 create(float _x, float _y, float _z, float _w)
	{
		float4 v;
		v.x= _x;
		v.y= _y;
		v.z= _z;
		v.w= _w;
		return v;
	};

	inline float3	xyz() { return {x, y, z}; }
	inline void		set(const float3& v)
	{
		x= v.x;
		y= v.y;
		z= v.z;
	}
};

float3 operator+ (const float3& lhs, const float3& rhs);
float3 operator- (const float3& lhs, const float3& rhs);
float3 operator* (const float3& lhs, const float3& rhs);
float3 operator/ (const float3& lhs, const float3& rhs);

float3 operator+ (const float3& v, float s);
float3 operator+ (float s, const float3& v);
float3 operator- (const float3& v, float s);
float3 operator- (float s, const float3& v);

float3 operator* (const float3& v, float s);
float3 operator* (float s, const float3& v);
float3 operator/ (const float3& v, float s);
float3 operator/ (float s, const float3& v);

float3 saturate(const float3& v);

inline float2 operator+ (const float2& v, float s)
{
	return { v.x+s, v.y+s };
}

inline float2 operator+ (float s, const float2& v)
{
	return {v.x+s, v.y+s };
}

inline float2 operator- (const float2& v, float s)
{
	return { v.x-s, v.y-s };
}

inline float2 operator- (float s, const float2& v)
{
	return {s-v.x, s-v.y };
}

inline float2 operator+ (const float2& lhs, const float2& rhs)
{
	return {lhs.x+rhs.x, lhs.y+rhs.y};
}

inline float2 operator- (const float2& lhs, const float2& rhs)
{
	return {lhs.x-rhs.x, lhs.y-rhs.y};
}

inline float2 operator* (const float2& lhs, const float2& rhs)
{
	return { lhs.x*rhs.x, lhs.y*rhs.y};
}

inline float2 operator/ (const float2& lhs, const float2& rhs)
{
	return { lhs.x/rhs.x, lhs.y/rhs.y};
}

inline float3 operator+ (const float3& lhs, const float3& rhs)
{
	return {lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z};
}

inline float3 operator- (const float3& lhs, const float3& rhs)
{
	return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z};
}

inline float3 operator* (const float3& lhs, const float3& rhs)
{
	return { lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z };
}

inline float3 operator/ (const float3& lhs, const float3& rhs)
{
	return { lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z};
}

inline float3 operator+ (const float3& v, float s)
{
	return { v.x+s, v.y+s, v.z+s };
}

inline float3 operator+ (float s, const float3& v)
{
	return {v.x+s, v.y+s, v.z+s };
}

inline float3 operator- (const float3& v, float s)
{
	return { v.x-s, v.y-s, v.z-s };
}

inline float3 operator- (float s, const float3& v)
{
	return {s-v.x, s-v.y, s-v.z};
}

inline float3 operator* (const float3& v, float s)
{
	return {v.x*s, v.y*s, v.z*s};
}

inline float3 operator* (float s, const float3& v)
{
	return {v.x*s, v.y*s, v.z*s};
}

inline float3 operator/ (const float3& v, float s)
{
	float invS= 1.0f/s;
	return {v.x*invS, v.y*invS, v.z*invS};
}

inline float3 operator/ (float s, const float3& v)
{
	return {s/v.x, s/v.y, s/v.z};
}

inline float3			float3::operator- () const
{
	return {-x, -y, -z};
}

inline float3&				float3::operator+= (const float3& v)
{
	x+= v.x;
	y+= v.y;
	z+= v.z;
	return *this;
}

inline float3&				float3::operator-= (const float3& v)
{
	x-= v.x;
	y-= v.y;
	z-= v.z;
	return *this;
}

inline float3&				float3::operator*= (const float3& v)
{
	x*= v.x;
	y*= v.y;
	z*= v.z;
	return *this;
}

inline float3&				float3::operator/= (const float3& v)
{
	x/= v.x;
	y/= v.y;
	z/= v.z;
	return *this;
}

inline float3&				float3::operator+= (float s)
{
	x+= s;
	y+= s;
	z+= s;
	return *this;
}

inline float3&				float3::operator-= (float s)
{
	x-= s;
	y-= s;
	z-= s;
	return *this;
}

inline float3&				float3::operator*= (float s)
{
	x*= s;
	y*= s;
	z*= s;
	return *this;
}

inline float3&				float3::operator/= (float s)
{
	float invS= 1.0f/ s;
	x*= invS;
	y*= invS;
	z*= invS;
	return *this;
}

