
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroSequence.h"
#include "MacroAction.h"
#include "keyboard/Keyboard.h"
#include "win.h"
#include <memory>
#include <stdarg.h>

MacroSequence::MacroSequence(const KeyboardState& triggerKey, float repeatTime, float firstRepeatDelayTime, int numActions, va_list argptr)
{
	m_triggerKey			= triggerKey;
	m_repeatTime			= repeatTime;
	m_repeatTimer			= 0.0f;
	m_firstRepeatDelayTime	= firstRepeatDelayTime;
	m_executingActionIdx	= -1;
	m_isExecuting			= false;
	MacroAction**	actionInst= (MacroAction**)malloc(sizeof(MacroAction*)*numActions);
	for(int i=0; i<numActions; ++i)
		actionInst[i]= va_arg(argptr, MacroAction*);
	m_actions	= actionInst;
	m_numAction	= numActions;
}

MacroSequence::~MacroSequence()
{
	end();
	int numActions= m_numAction;
	for(int i=0; i<numActions; ++i)
		delete m_actions[i];
	free(m_actions);
}

bool	MacroSequence::start()
{
	assert(m_numAction > 0);
	storeModifier();
	m_repeatTimer	= m_repeatTime + m_firstRepeatDelayTime;
	m_isExecuting	= true;
	bool isEnd= startAtIdx(0);
	if (isEnd && m_repeatTime == 0.0f)
		end();
	return isEnd;
}

bool	MacroSequence::startAtIdx(int idx)
{
	int		num				= m_numAction;
	bool	isContinueAction= true;
	MacroAction* action= m_actions[idx];
	while (isContinueAction)
	{
		if (action->start())
		{
			action->end();
			++idx;
			if (idx < num)
				action= m_actions[idx];
			else
			{
				isContinueAction= false;
				idx				= -1;
			}
		}
		else
			isContinueAction= false;
	}

	m_executingActionIdx	= idx;
	return idx == -1;
}

bool	MacroSequence::update(float deltaSeconds)
{
	if (m_repeatTime > 0)
	{
		// repeat action
		assert(m_executingActionIdx	== -1);
		m_repeatTimer-= deltaSeconds;
		if (m_repeatTimer <= 0.0f)
		{
			// repeat action
			bool isEnd= startAtIdx(0);
			assert(isEnd);	// hold repeat action must end within a frame for simplicity
			m_repeatTimer+= m_repeatTime;
		}
		return false;
	}
	else
	{
		// conitnue execution
		assert(m_executingActionIdx	!= -1);
		MacroAction* action= m_actions[m_executingActionIdx];
		if (action->update(deltaSeconds))
		{
			action->end();
			++m_executingActionIdx;
			if (m_executingActionIdx < m_numAction)
			{
				bool isEnd= startAtIdx(m_executingActionIdx);
				if (isEnd)
					end();
				return isEnd;
			}
			else
				return false;
		}
		else
			return false;
	}
}

void	MacroSequence::end()
{
	if (!m_isExecuting)
		return;
	if (m_executingActionIdx != -1)
	{
		m_actions[m_executingActionIdx]->end();
		m_executingActionIdx= -1;
	}
	restoreModifier();
	m_isExecuting	= false;
}

void	MacroSequence::storeModifier()
{
	setModifier(KEYEVENTF_KEYUP);
}

void	MacroSequence::restoreModifier()
{
//	setModifier(0);
}

void	MacroSequence::setModifier(unsigned int flag)
{
	InputKey		modifers[]			= { InputKey::Shift_Left, InputKey::Shift_Right	, InputKey::Control_Left	, InputKey::Control_Right, InputKey::Alt_Left	, InputKey::Alt_Right };
	unsigned int	modifierScanCode[]	= { 0x2a					, 0x36						, 0x1d						, 0x1d						, 0x38					, 0x38					};
	unsigned int	modifersFlag[]		= { 0						, 0							, 0							, KEYEVENTF_EXTENDEDKEY		, 0						, KEYEVENTF_EXTENDEDKEY	};
	for(int i=0; i<_countof(modifers); ++i)
		if (m_triggerKey.isKeyOn(modifers[i]))
		{
			INPUT input;
			input.type				= INPUT_KEYBOARD;
			input.ki.wVk			= 0;
			input.ki.wScan			= modifierScanCode[i];
			input.ki.dwFlags		= modifersFlag[i] | flag | KEYEVENTF_SCANCODE;
			input.ki.time			= 0;
			input.ki.dwExtraInfo	= 0;
			SendInput(1, &input, sizeof(input));
		}
}

bool	MacroSequence::keyStateChanged(Keyboard* keyboard, InputKey key, bool isPressed)
{
	if (m_isExecuting)
	{
		bool isTriggerKeys	= isTriggerKey(key);
		bool isRepeat		= m_repeatTime > 0;
		bool isEnd			=	(!isRepeat && !isTriggerKeys) ||
								( isRepeat &&  isTriggerKeys);
		if (isEnd)
		{
			end();
			return true;
		}
		else
			return false;
	}
	else
	{
		// start macro sequence if trigger key match
		if (keyboard->isExactKeyDown(m_triggerKey))
		{
			bool isEnd		= start();
			bool isRepeat	= m_repeatTime > 0.0f;
			assert(!isRepeat || isEnd);	// hold repeat action must end within a frame for simplicity
			return !isEnd || isRepeat;
		}
		else
			return false;
	}
}
