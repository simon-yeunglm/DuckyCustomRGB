#pragma once

// by simon yeung, 26/10/2020
// all rights reserved

#include "../RenderPass.h"

enum class InputKey;

class RenderPass_AudioVolumeBar : public RenderPass
{
protected:
	float	m_volume;
	float	m_blinkTimer;
	float	m_blinkSpeed;
	float	m_timer;
	float	m_alpha;
	float	m_fadeInSpeed;
	float	m_fadeOutTime;
	float	m_totalDisplayTime;
	
	virtual void updatePass(	Keyboard* keyboard, float deltaSeconds)override;
	virtual void renderPass(	const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;

	void	renderDigit(int digit, float alpha, const InputKey displayKeys[9],const RenderLayout* layout, RenderTarget* outputRenderTarget);
	
public:
	RenderPass_AudioVolumeBar(float fadeInTime, float displayTime, float fadeOutTime, RenderPass* underlyingPass);
	virtual ~RenderPass_AudioVolumeBar();
	
	void	volumeCallback(float volumne);
};

