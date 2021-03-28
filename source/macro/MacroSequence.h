#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

#include "keyboard/KeyboardState.h"

class Keyboard;
class MacroAction;

class MacroSequence
{
private:
	MacroAction**	m_actions;
	int				m_numAction;
	int				m_executingActionIdx;
	bool			m_isExecuting;
	KeyboardState	m_triggerKey;
	float			m_repeatTime;			// the action repeat time, == 0 => trigger action when key down
											//							> 0 => repeat action every m_repeatTimer seconds until ket released
	float			m_firstRepeatDelayTime;	// delay repeat time for the first repeat
	float			m_repeatTimer;

	MacroSequence()= default;

	// return whether the action is ended
	bool	start();
	void	end();

	bool	startAtIdx(int idx);
	
	void	storeModifier();
	void	restoreModifier();
	void	setModifier(unsigned int flag);

public:
	MacroSequence(const KeyboardState& triggerKey, float repeatTime, float firstRepeatDelayTime, int numActions, va_list argptr);
	~MacroSequence();
	
	bool	update(float deltaSeconds);

	// when not executed	return whether the sequence started
	// otherwise			return whether the sequence is ended
	bool	keyStateChanged(Keyboard* keyboard, InputKey key, bool isPressed);

	inline bool isExecuting()const	{ return m_isExecuting; }
	inline bool	isTriggerKey(InputKey key)const
	{
		return m_triggerKey.isKeyOn(key);
	}
};

