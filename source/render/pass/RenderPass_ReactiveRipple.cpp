
// by simon yeung, 25/10/2020
// all rights reserved

#include "RenderPass_ReactiveRipple.h"
#include "render/RenderLayout.h"
#include "render/RenderTarget.h"
#include "keyboard/Keyboard.h"
#include "math/Math.h"


RenderPass_ReactiveRipple::RenderPass_ReactiveRipple(const float3& keyDownColor, const float3& fadeOutColor, float rippleRadiusMax, float rippleHalfWidth, float fadeOutTIme, const KeyboardState& triggerKey, RenderPass* underlyingPass)
	:RenderPass_ReactiveKey(keyDownColor, fadeOutTIme, triggerKey, underlyingPass)
{
	m_rippleFadeOutColor	= fadeOutColor;
	m_rippleRadiusMax		= rippleRadiusMax * NUM_PIXEL_PER_KEY;
	m_rippleHalfWidth		= rippleHalfWidth * NUM_PIXEL_PER_KEY;
	m_isUniqueFadeOutKey	= false;
	m_desireFrameRate		= 20.0f;
}

RenderPass_ReactiveRipple::~RenderPass_ReactiveRipple()
{
}
	

void RenderPass_ReactiveRipple::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
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
	float	maxRadius		= m_rippleRadiusMax;
	float	halfWidth		= m_rippleHalfWidth;
	float3	fadeOutColor	= m_rippleFadeOutColor;
	int		numFade			= m_fadeKey.num();
	for(int i=0; i<numFade; ++i)
	{
		FadeOutKey& fadeOutKey		= m_fadeKey[i];
		float		colorAlpha		= fadeOutKey.fadeOut;
		float		rippleAlpha		= Math::sqrt(colorAlpha);
		float		t				= 1.0f - rippleAlpha;
		int4		pos				= layout->keyPixelPos[(int)fadeOutKey.key];
		int			numPx			= (pos.z - pos.x) * (pos.w - pos.y);
		if (numPx <= 0)
			continue;

		float2		keyCenter		= { (pos.z + pos.x) * 0.5f,  (pos.w + pos.y) * 0.5f };
		float		radius			= maxRadius * t;
		float		outerRadius		= radius + halfWidth;
		float		innerRadius		= radius - halfWidth;	// no need to clamp <= 0, as the for loop condition will skip those pixels
		float2		rippleOuterMin	= keyCenter - outerRadius;
		float2		rippleOuterMax	= keyCenter + outerRadius;
		float2		rippleInnerMin	= keyCenter - innerRadius;
		float2		rippleInnerMax	= keyCenter + innerRadius;
		int4		rippleBoundOuter= { (int)Math::floor(	rippleOuterMin.x), 
										(int)Math::floor(	rippleOuterMin.y), 
										(int)Math::ceil(	rippleOuterMax.x), 
										(int)Math::ceil(	rippleOuterMax.y)
									};
		int4		rippleBoundInner= { (int)Math::ceil(	rippleInnerMin.x), 
										(int)Math::ceil(	rippleInnerMin.y), 
										(int)Math::floor(	rippleInnerMax.x), 
										(int)Math::floor(	rippleInnerMax.y)
									};
		rippleBoundOuter.x= Math::max(rippleBoundOuter.x, 0			);
		rippleBoundOuter.y= Math::max(rippleBoundOuter.y, 0			);
		rippleBoundOuter.z= Math::min(rippleBoundOuter.z, RTsz.x	);
		rippleBoundOuter.w= Math::min(rippleBoundOuter.w, RTsz.y	);

		for(int y=rippleBoundOuter.y; y<rippleBoundOuter.w; ++y)
			for(int x=rippleBoundOuter.x; x<rippleBoundOuter.z; ++x)
			{
				bool isInInnerBoundr=	x > rippleBoundInner.x && y > rippleBoundInner.y &&
										x < rippleBoundInner.z && y < rippleBoundInner.w ;
				if (isInInnerBoundr)
					continue;

				float2	pxPos		= { (float)x, (float)y };
				float	disToCenter	= ( pxPos -keyCenter ).length();
				float	disToRipple	= Math::abs(radius - disToCenter);
				float	rippleT		= Math::clamp(disToRipple / halfWidth, 0.0f, 1.0f);
				rippleT				= 1.0f - rippleT * rippleT;

				float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
				float3 rippleColor	= Math::lerp(fadeOutColor, downColor, colorAlpha);
				rippleColor			*= rippleT;	// darken the edge to make the ripple effect pop
				float3 blendColor	= Math::lerp(originalColor, rippleColor, rippleAlpha);
				outputRenderTarget->setPixel(x, y, blendColor);
			}
	}
}
