#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

#include "collection/Array.h"

class MacroSequence;
class Keyboard;
struct KeyboardState;
enum class KeyboardKey;

class MacroPlayer
{
private:
	Array<MacroSequence*>	m_sequences;
	int						m_executingSequenceIdx;

public:
	MacroPlayer();
	~MacroPlayer();
	
	// reutrn desire FPS
	float	update(float deltaSeconds);
	void	addMacro(const KeyboardState& triggerKey, float repeatTime, float firstRepeatDelayTime, int numAction, ...);
	void	keyStateChanged(Keyboard* keyboard, KeyboardKey key, bool isPressed);
};
