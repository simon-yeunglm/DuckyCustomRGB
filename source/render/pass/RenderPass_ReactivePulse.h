#pragma once

// by simon yeung, 30/10/2020
// all rights reserved

#include "RenderPass_ReactiveKey.h"

class RenderPass_ReactivePulse : public RenderPass_ReactiveKey
{
protected:
	float3	m_pulseColor;
	float	m_pulseDistance;	// in unit of number of pixel

	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
public:
	// pulseTravelDistance in unit of number of key caps, can be positive/negative to indicate left/right direction
	RenderPass_ReactivePulse(const float3& keyDownColor, const float3& pulseColor, float pulseTravelDistance, float pulseTIme, const KeyboardState& triggerKey= KeyboardState(0xfffff), RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_ReactivePulse();
};
