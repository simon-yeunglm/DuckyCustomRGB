
// by simon yeung, 26/10/2020
// all rights reserved

#include <assert.h>
#include "RenderPass_AudioVolumeBar.h"
#include "math/Math.h"
#include "math/Color.h"
#include "render/RenderTarget.h"
#include "render/RenderLayout.h"
#include "audio/Audio.h"

static void RenderPass_AudioVolumeBar_volumeCallback(float normalizedVolume, void* userPtr)
{
	RenderPass_AudioVolumeBar* pass= (RenderPass_AudioVolumeBar*)userPtr;
	pass->volumeCallback(normalizedVolume);
}

RenderPass_AudioVolumeBar::RenderPass_AudioVolumeBar(float fadeInTime, float displayTime, float fadeOutTime, RenderPass* underlyingPass)
	:RenderPass(underlyingPass)
{
	m_volume			= 0.0f;
	m_blinkTimer		= 0.0f;
	m_blinkSpeed		= 2.0f;
	m_timer				= 0.0f;
	m_alpha				= 0.0f;
	m_fadeInSpeed		= 1.0f / fadeInTime;
	m_fadeOutTime		= fadeOutTime;
	m_totalDisplayTime	= fadeInTime + displayTime + fadeOutTime;
	m_desireFrameRate	= 15.0f;
	Audio::registerVolumeCallback(&RenderPass_AudioVolumeBar_volumeCallback, this);
}

RenderPass_AudioVolumeBar::~RenderPass_AudioVolumeBar()
{
	Audio::deregisterVolumeCallback(&RenderPass_AudioVolumeBar_volumeCallback, this);
}

void RenderPass_AudioVolumeBar::updatePass(	Keyboard* keyboard, float deltaSeconds)
{
	m_timer				= Math::max(m_timer - deltaSeconds, 0.0f);
	float changeSpeed	= m_timer < m_fadeOutTime ? -1.0f / m_fadeOutTime : m_fadeInSpeed;
	changeSpeed			*= deltaSeconds;
	m_alpha				= Math::clamp(m_alpha + changeSpeed, 0.0f, 1.0f);
	m_blinkTimer		= Math::mod(m_blinkTimer + m_blinkSpeed * deltaSeconds, 1.0f);

	// update render pass state
	bool isDisplaying= m_alpha > 0;
	unsigned int flag= m_flag;
	m_flag= isDisplaying ? (flag & ~RENDER_PASS_FLAG_FULLY_TRANSPARENT) : (flag | RENDER_PASS_FLAG_FULLY_TRANSPARENT);
}

void RenderPass_AudioVolumeBar::renderPass(	const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	// volume bar pixel coordinates
	int barX_start	= 4;
	int barY_start	= 4;
	int barX_end	= 44;
	int barY_end	= 8;
	int2		RTsz	= outputRenderTarget->size;
	barX_start			= Math::max(barX_start	, 0		);
	barY_start			= Math::max(barY_start	, 0		);
	barX_end			= Math::min(barX_end	, RTsz.x	);
	barY_end			= Math::min(barY_end	, RTsz.y	);

	// darken background
	float darkenScale= 0.6;
	float alpha		= m_alpha;
	for(int y=0; y<RTsz.y; ++y)
		for(int x=0; x<RTsz.x; ++x)
		{
			float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
			float3	HSV			= Color::RGBtoHSV(originalColor);
			HSV.z				*= darkenScale;
			float3 darkenColor	= Color::HSVtoRGB( HSV );
			float3 blendColor	= Math::lerp(originalColor, darkenColor, alpha);
			outputRenderTarget->setPixel(x, y, blendColor);
		}

	// compute bar width
	float barWidth= (barX_end - barX_start) * m_volume;
	if (barWidth <= 0.0f)
		return;
	barX_end= (int)(barX_start + barWidth);
	
	// render volume bar
	float3 startColor	= Color::RGBtoHSV({0.0f, 0.7f, 1.0f });
	float3 endColor		= Color::RGBtoHSV({1.0f, 0.0f, 0.005f});
	float  colorDelta	= 1.0f/ (float)(barX_end-barX_start-1);
	for(int x=barX_start; x<barX_end; ++x)
	{
		float  t			= (x -barX_start) * colorDelta;
		t					*= t*t;//*t;
		float3	barColor	= Math::lerp(startColor, endColor, t);
		barColor			= Color::HSVtoRGB(barColor);

		for(int y=barY_start; y<barY_end; ++y)
		{	
			float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
			float3 blendColor	= Math::lerp(originalColor, barColor, alpha);
			outputRenderTarget->setPixel(x, y, blendColor);
		}
	}

	/* disable it because the effect looks not good on the keyboard
	// render volume digits
	int volumePercent= (int)Math::round(m_volume * 100.0f);
	int digit0	= volumePercent			% 10;
	int digit1	= (volumePercent / 10 )	% 10;
	int digit2	= (volumePercent / 100)	% 10;

	KeyboardKey keyDigit0[9]= { KeyboardKey::I		, KeyboardKey::O	, KeyboardKey::P,
								KeyboardKey::K		, KeyboardKey::L	, KeyboardKey::Colon,
								KeyboardKey::Comma	, KeyboardKey::Dot	, KeyboardKey::ForwardSlash};
	KeyboardKey keyDigit1[9]= { KeyboardKey::R		, KeyboardKey::T	, KeyboardKey::Y,
								KeyboardKey::F		, KeyboardKey::G	, KeyboardKey::H,
								KeyboardKey::V		, KeyboardKey::B	, KeyboardKey::N};
	KeyboardKey keyDigit2[9]= { KeyboardKey::Q		, KeyboardKey::W	, KeyboardKey::E,
								KeyboardKey::A		, KeyboardKey::S	, KeyboardKey::D,
								KeyboardKey::Z		, KeyboardKey::X	, KeyboardKey::C};
	if (volumePercent >= 100)
		renderDigit(digit2, alpha, keyDigit2, layout, outputRenderTarget);
	if (volumePercent >= 10)
		renderDigit(digit1, alpha, keyDigit1, layout, outputRenderTarget);
	renderDigit(digit0, alpha, keyDigit0, layout, outputRenderTarget);
	*/
}

void	RenderPass_AudioVolumeBar::renderDigit(int digit, float alpha, const KeyboardKey displayKeys[9],const RenderLayout* layout, RenderTarget* outputRenderTarget)
{
	assert(digit < 10);
	int onKeyIdx[9];
	int numKey= 0;
	if (digit== 0)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 3;
		onKeyIdx[numKey++]= 5;
		onKeyIdx[numKey++]= 6;
		onKeyIdx[numKey++]= 7;
		onKeyIdx[numKey++]= 8;
	}
	else if (digit== 1)
	{
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 7;
	}
	else if (digit== 2)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 6;
		onKeyIdx[numKey++]= 7;
		onKeyIdx[numKey++]= 8;
	}
	else if (digit== 3)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 5;
		onKeyIdx[numKey++]= 6;
		onKeyIdx[numKey++]= 7;
		onKeyIdx[numKey++]= 8;
	}
	else if (digit== 4)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 3;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 5;
		onKeyIdx[numKey++]= 8;
	}
	else if (digit== 5)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 3;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 6;
		onKeyIdx[numKey++]= 7;
		onKeyIdx[numKey++]= 8;
	}
	else if (digit== 6)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 3;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 6;
		onKeyIdx[numKey++]= 7;
	}
	else if (digit== 7)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 6;
	}
	else if (digit== 8)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 3;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 5;
		onKeyIdx[numKey++]= 6;
		onKeyIdx[numKey++]= 7;
		onKeyIdx[numKey++]= 8;
	}
	else if (digit== 9)
	{
		onKeyIdx[numKey++]= 0;
		onKeyIdx[numKey++]= 1;
		onKeyIdx[numKey++]= 2;
		onKeyIdx[numKey++]= 3;
		onKeyIdx[numKey++]= 4;
		onKeyIdx[numKey++]= 5;
		onKeyIdx[numKey++]= 8;
	}
	else
		assert(false);

	for(int i=0; i<numKey; ++i)
	{
		KeyboardKey key= displayKeys[onKeyIdx[i]];
		int4 pos= layout->keyPixelPos[(int)key];
		for(int y=pos.y; y<pos.w; ++y)
			for(int x=pos.x; x<pos.z; ++x)
			{
				float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
				float3 barColor		= {1,1,1};
				float3 blendColor	= Math::lerp(originalColor, barColor, alpha);
				outputRenderTarget->setPixel(x, y, blendColor);
			}
	}
}

void	RenderPass_AudioVolumeBar::volumeCallback(float volume)
{
	m_volume= volume;
	m_timer= m_totalDisplayTime;
}
