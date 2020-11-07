
// by simon yeung, 30/10/2020
// all rights reserved

#include "RenderPass_ReactivePulse.h"
#include "render/RenderLayout.h"
#include "render/RenderTarget.h"
#include "keyboard/Keyboard.h"
#include "math/Math.h"

RenderPass_ReactivePulse::RenderPass_ReactivePulse(const float3& keyDownColor, const float3& pulseColor, float pulseTravelDistance, float pulseTIme, const KeyboardState& triggerKey, RenderPass* underlyingPass)
	:RenderPass_ReactiveKey(keyDownColor, pulseTIme, triggerKey, underlyingPass)
{
	m_pulseColor			= pulseColor;
	m_pulseDistance			= pulseTravelDistance * NUM_PIXEL_PER_KEY;
	m_isUniqueFadeOutKey	= false;
	m_desireFrameRate		= 20.0f;
}

RenderPass_ReactivePulse::~RenderPass_ReactivePulse()
{
}
	

void RenderPass_ReactivePulse::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	// render down key
	float3		borderColor	= {0, 0, 0};	// darken the pressed key surrounding to make the pressed color pop
	const int	border		= 1;			// < NUM_PIXEL_PER_KEY will blend in some original color, as each key size NUM_PIXEL_PER_KEY x NUM_PIXEL_PER_KEY
	int2		RTsz		= outputRenderTarget->size;
	float3		downColor	= m_keyDownColor;
	int			numDown		= m_downKey.num();
	for(int i=0; i<numDown; ++i)
	{
		int4 pos= layout->keyPixelPos[(int)m_downKey[i]];
		int4 expandedRange	= pos;
		expandedRange.x= Math::max(expandedRange.x - border, 0		);
		expandedRange.y= Math::max(expandedRange.y - border, 0		);
		expandedRange.z= Math::min(expandedRange.z + border, RTsz.x	);
		expandedRange.w= Math::min(expandedRange.w + border, RTsz.y	);
		
		for(int y=expandedRange.y; y<expandedRange.w; ++y)
			for(int x=expandedRange.x; x<expandedRange.z; ++x)
			{
				bool isBorder=  x < pos.x || y < pos.y ||
								x >=pos.z || y >=pos.w ;
				outputRenderTarget->setPixel(x, y, isBorder ? borderColor : downColor);
			}
	}

	// render fade out
	float	pulseDistance	= m_pulseDistance;
	float3	pulseColor		= m_pulseColor;
	int		numFade			= m_fadeKey.num();
	bool	isRightDir		= m_pulseDistance > 0;
	for(int i=0; i<numFade; ++i)
	{
		FadeOutKey& fadeOutKey		= m_fadeKey[i];
		float		colorAlpha		= fadeOutKey.fadeOut;
		float		t				= 1.0f - colorAlpha;
		colorAlpha					= Math::sqrt(colorAlpha);
		colorAlpha					= Math::sqrt(colorAlpha);
		int4		pos				= layout->keyPixelPos[(int)fadeOutKey.key];
		int			height			= pos.w - pos.y;
		int			numPx			= (pos.z - pos.x) * height;
		if (numPx <= 0)
			continue;

		float		t2				= t * t;
		float2 pulseCenter;
		pulseCenter.x		= (float)(isRightDir ? pos.x : pos.z);
		pulseCenter.x		+= pulseDistance * t;
		pulseCenter.y		= (pos.w + pos.y) * 0.5f;

		int4 expandedRange	= {	(int)Math::floor(	pulseCenter.x - height),
								(int)Math::floor(	pulseCenter.y - height), 
								(int)Math::ceil(	pulseCenter.x + height),  
								(int)Math::ceil(	pulseCenter.y + height)};
		expandedRange.x= Math::max(expandedRange.x - border, 0		);
		expandedRange.y= Math::max(expandedRange.y - border, 0		);
		expandedRange.z= Math::min(expandedRange.z + border, RTsz.x	);
		expandedRange.w= Math::min(expandedRange.w + border, RTsz.y	);
		
		for(int y=expandedRange.y; y<=expandedRange.w; ++y)
			for(int x=expandedRange.x; x<=expandedRange.z; ++x)
			{
				float2	pxPos		= { (float)x, (float)y };
				float	disToCenter	= ( pxPos - pulseCenter ).length();
				float	T			= Math::clamp(disToCenter / height, 0.0f, 1.0f);
				T					*= T;
				float	alpha		= (1.0f - T*T*T*T) * colorAlpha;

				float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
				bool isBorder=  x ==expandedRange.x || y ==expandedRange.y ||
								x ==expandedRange.z || y ==expandedRange.w ;
				float3 blendColor	= Math::lerp(originalColor, isBorder ? borderColor : pulseColor, alpha);
				outputRenderTarget->setPixel(x, y, blendColor);
			}
	}
}
