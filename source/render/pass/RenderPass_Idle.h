#pragma once

// by simon yeung, 26/10/2020
// all rights reserved

#include "../RenderPass.h"
#include "math/vector.h"
#include "keyboard/KeyboardState.h"
#include "collection/Array.h"

class RenderPass_Idle : public RenderPass
{
protected:
	enum class State
	{
		Idle,
		WakingUpFromIdle,
		WakingUpFromEnteringIdle,
		Awake,
		EnteringIdle,
		Num
	};

	State	m_state;
	float	m_stateTimer;
	float	m_idleTime;		// in seconds
	float	m_idleTimer;
	float	m_wakeUpSlashTime;
	float	m_wakeUpShowVertLineTime;
	float	m_wakeUpMoveVertLineTime;
	float	m_enterIdleTime;

	virtual void bindKeyboard(  Keyboard* keyboard)override;
	virtual void unbindKeyboard(Keyboard* keyboard)override;

	virtual void updatePass(Keyboard* keyboard, float deltaSeconds)override;
	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
	void	changeState(State newState);
	void	updateWakingUpFromIdle(			float deltaSeconds);
	void	updateWakingUpFromEnteringIdle(	float deltaSeconds);
	void	updateAwake(					float deltaSeconds);
	void	updateEnteringIdle(				float deltaSeconds);
	
	void	renderIdle(				RenderTarget* outputRenderTarget);
	void	renderWakingUpFromIdle(	RenderTarget* outputRenderTarget);
	void	renderEnteringIdle(		RenderTarget* outputRenderTarget);

public:
	RenderPass_Idle(float idleTime, RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_Idle();
	
	virtual float update(	Keyboard* keyboard, float deltaSeconds)override;

	void OnKeyPressed(InputKey key, bool isPressed);
};
