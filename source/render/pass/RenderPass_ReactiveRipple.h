#pragma once

// by simon yeung, 25/10/2020
// all rights reserved

#include "RenderPass_ReactiveKey.h"

class RenderPass_ReactiveRipple : public RenderPass_ReactiveKey
{
protected:
	float3	m_rippleFadeOutColor;
	float	m_rippleRadiusMax;	// in unit of number of pixel
	float	m_rippleHalfWidth;	// in unit of number of pixel

	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
public:
	// rippleRadiusMax and rippleHalfWidth in unit of number of key caps
	RenderPass_ReactiveRipple(const float3& keyDownColor, const float3& fadeOutColor, float rippleRadiusMax, float rippleHalfWidth, float fadeOutTIme, const KeyboardState& triggerKey= KeyboardState(0xfffff), RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_ReactiveRipple();
};
