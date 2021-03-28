#pragma once

// by simon yeung, 24/10/2020
// all rights reserved

#include <assert.h>
#include "KeyboardKey.h"

struct KeyboardState
{
private:
	unsigned int	keyIsOn[4];

	inline void getIdx(InputKey key, int* arrayIdx, int* bitIdx)const
	{
		int idx		= (int)key;
		*arrayIdx	= idx / 32;
		*bitIdx		= idx % 32;
	}

public:
	KeyboardState()= default;
	
	KeyboardState(InputKey onKey, bool isOn= true):
		KeyboardState(&onKey, 1, isOn)
	{
	}
	
	KeyboardState(const InputKey* onKeys, int numOnKey, bool isOn= true):KeyboardState(isOn ? 0 : 0xffffffff)
	{
		for(int i=0; i<numOnKey; ++i)
			setKeyIsOn(onKeys[i], isOn);
	}

	KeyboardState(unsigned int defaultKey)
	{

//		// below code works, but generate annoying intellisense warnings C26495
//		// and can't be suppressed so unroll the loop manually instead
//		for(int i=0; i<sizeof(keyIsOn)/sizeof(int); ++i)
//			keyIsOn[i]= defaultKey;
		static_assert(sizeof(keyIsOn)/sizeof(int) == 4, "update below initialization");
		keyIsOn[0]= defaultKey;
		keyIsOn[1]= defaultKey;
		keyIsOn[2]= defaultKey;
		keyIsOn[3]= defaultKey;
	}
	
	inline void	setAllOff()
	{
		for(int i=0; i<sizeof(keyIsOn)/sizeof(int); ++i)
			keyIsOn[i]= 0;
	}
	inline void	setAllOn()
	{
		for(int i=0; i<sizeof(keyIsOn)/sizeof(int); ++i)
			keyIsOn[i]= 0xffffffff;
	}
	// return is changed
	inline bool	setKeyOn(InputKey key)
	{
		int arrayIdx;
		int bitIdx;
		getIdx(key, &arrayIdx, &bitIdx);
		unsigned int shiftedBit	= 1 << bitIdx;
		unsigned int lastKey	= keyIsOn[arrayIdx];
		unsigned int newKey		= lastKey | shiftedBit;
		keyIsOn[arrayIdx]		= newKey;
		return lastKey != newKey;
	}
	inline bool	setKeyOff(InputKey key)
	{
		int arrayIdx;
		int bitIdx;
		getIdx(key, &arrayIdx, &bitIdx);
		unsigned int shiftedBit	= 1 << bitIdx;
		unsigned int lastKey	= keyIsOn[arrayIdx];
		unsigned int newKey		= lastKey & (~shiftedBit);
		keyIsOn[arrayIdx]		= newKey;
		return lastKey != newKey;
	}
	inline bool	setKeyIsOn(InputKey key, bool isOn)
	{
		int arrayIdx;
		int bitIdx;
		getIdx(key, &arrayIdx, &bitIdx);
		unsigned int shiftedBit	= 1 << bitIdx;
		unsigned int lastKey	= keyIsOn[arrayIdx];
		unsigned int newKey		= isOn ? (lastKey | shiftedBit) : (lastKey & (~shiftedBit));
		keyIsOn[arrayIdx]		= newKey;
		return lastKey != newKey;
	}
	inline bool isKeyOn(InputKey key)const
	{
		int arrayIdx;
		int bitIdx;
		getIdx(key, &arrayIdx, &bitIdx);
		return (keyIsOn[arrayIdx] & (1 << bitIdx)) != 0;
	}

	inline bool isAnyKeyOn(const KeyboardState& key)const
	{
		unsigned int isOn= 0;
		for(int i=0; i<sizeof(keyIsOn)/sizeof(int); ++i)
			isOn|= keyIsOn[i] & key.keyIsOn[i];
		return isOn != 0;
	}

	inline bool operator==(const KeyboardState& RHS)const
	{
		bool isEqual= true;
		for(int i=0; i<sizeof(keyIsOn)/sizeof(int); ++i)
			isEqual= isEqual && keyIsOn[i] == RHS.keyIsOn[i];
		return isEqual;
	}
};

static_assert( sizeof(KeyboardState) * 8 >= (int)InputKey::Num, "Not enough bit flags for all keys.");
