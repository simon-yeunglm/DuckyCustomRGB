
// by simon yeung, 31/10/2020
// all rights reserved

#include "MacroPlayer.h"
#include "MacroSequence.h"
#include <stdarg.h>

MacroPlayer::MacroPlayer()
{
	m_executingSequenceIdx= -1;
}

MacroPlayer::~MacroPlayer()
{
	int numSeq= m_sequences.num();
	for(int i=0; i<numSeq; ++i)
		delete m_sequences[i];
	m_sequences.clear();
}

void	MacroPlayer::addMacro(const KeyboardState& triggerKey, float repeatTime, float firstRepeatDelayTime, int numAction, ...)
{
	va_list argptr;
	va_start(argptr, numAction);
	MacroSequence* seq= new MacroSequence(triggerKey, repeatTime, firstRepeatDelayTime, numAction, argptr);
	m_sequences.add(seq);
	va_end(argptr);	

}

float	MacroPlayer::update(float deltaSeconds)
{
	if (m_executingSequenceIdx != -1)
	{
		MacroSequence* seq= m_sequences[m_executingSequenceIdx];
		if (seq->update(deltaSeconds))
			m_executingSequenceIdx= -1;
		return 30.0f;
	}
	else
		return 0.0f;
}

void	MacroPlayer::keyStateChanged(Keyboard* keyboard, KeyboardKey key, bool isPressed)
{
	if (m_executingSequenceIdx != -1)
	{
		// stop executing macro
		MacroSequence* seq= m_sequences[m_executingSequenceIdx];
		assert(seq->isExecuting());
		if (seq->keyStateChanged(keyboard, key, isPressed))
			m_executingSequenceIdx= -1;
	}
	else
	{
		// try start a new macro
		int numSeq= m_sequences.num();
		for(int i=0; i<numSeq; ++i)
			if (m_sequences[i]->keyStateChanged(keyboard, key, isPressed))
			{
				m_executingSequenceIdx= i;
				break;
			}
	}
}
