#pragma once

// by simon yeung, 24/10/2020
// all rights reserved

#include "../RenderPass.h"
#include "math/vector.h"
#include "keyboard/KeyboardState.h"
#include "collection/Array.h"

class RenderPass_ReactiveKey : public RenderPass
{
protected:
	struct FadeOutKey
	{
		InputKey	key;
		float		fadeOut;
	};

	float3				m_keyDownColor;
	float				m_fadeOutSpeed;
	KeyboardState		m_triggerKey;
	Array<InputKey>	m_downKey;
	Array<FadeOutKey>	m_fadeKey;
	bool				m_isUniqueFadeOutKey;
	
	virtual void bindKeyboard(  Keyboard* keyboard)override;
	virtual void unbindKeyboard(Keyboard* keyboard)override;

	virtual void updatePass(Keyboard* keyboard, float deltaSeconds)override;
	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
public:
	RenderPass_ReactiveKey(const float3& keyDownColor, float fadeOutTIme, const KeyboardState& triggerKey= KeyboardState(0xfffff), RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_ReactiveKey();

	void OnKeyPressed(InputKey key, bool isPressed);
};
