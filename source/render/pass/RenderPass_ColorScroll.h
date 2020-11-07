#pragma once

// by simon yeung, 18/10/2020
// all rights reserved

#include "../RenderPass.h"

struct float3;

// color scroll horizontally
class RenderPass_ColorScroll : public RenderPass
{
protected:
	float3*	m_scrollColor;
	int		m_scrollColorNum;
	float	m_speed;
	int		m_currentColorIdx;
	float	m_indexOffset;	// between adjust pixel
	float	m_timer;

	virtual void updatePass(Keyboard* keyboard, float deltaSeconds)override;
	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
public:
	// distanceBetweenColor = number of key cap between m_scrollColor[n] and m_scrollColor[n+1]
	RenderPass_ColorScroll(const float3* scrollColor, int scrollColorNum, float distanceBetweenColor, float changeColorTime, RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass_ColorScroll();
};
