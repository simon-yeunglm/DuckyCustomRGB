#pragma once

// by simon yeung, 25/10/2020
// all rights reserved

#include "../RenderPass.h"

enum class KeyboardKey;

class RenderPass_KeyLockBlink : public RenderPass
{
protected:
	float		m_BlinkSpeed;
	float		m_blinkTimer;
	bool		m_isLockCaps;
	bool		m_isLockScroll;
	bool		m_isLockNum;	// blinking num lock is a bit annoying, so this variable is not used

	virtual void bindKeyboard(  Keyboard* keyboard)override;
	virtual void unbindKeyboard(Keyboard* keyboard)override;

	virtual void updatePass(Keyboard* keyboard, float deltaSeconds)override;
	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
	void	updateLockCaps();
	void	updateLockScroll();
	void	updateLockNum();

	void	renderBlink(KeyboardKey key, const RenderLayout* layout, RenderTarget* outputRenderTarget);

public:
	RenderPass_KeyLockBlink(float blinkSpeed, RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_KeyLockBlink();
	
	void	OnKeyPressed(KeyboardKey key, bool isPressed);
};
