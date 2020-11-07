#pragma once

// by simon yeung, 18/10/2020
// all rights reserved

#include "../RenderPass.h"

struct float3;

class RenderPass_ColorBreath : public RenderPass
{
protected:
	float3*	m_breathColor;
	int		m_breathColorNum;
	float	m_speed;
	int		m_currentColorIdx;
	float	m_timer;

	virtual void updatePass(Keyboard* keyboard, float deltaSeconds)override;
	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
public:
	RenderPass_ColorBreath(const float3* breathColor, int breathColorNum, float changeColorTime, RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_ColorBreath();
};
