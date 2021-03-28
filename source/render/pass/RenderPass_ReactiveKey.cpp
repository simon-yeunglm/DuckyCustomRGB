
// by simon yeung, 24/10/2020
// all rights reserved

#include "RenderPass_ReactiveKey.h"
#include "render/RenderLayout.h"
#include "render/RenderTarget.h"
#include "keyboard/Keyboard.h"
#include "math/Math.h"

static void RenderPass_ReactiveKey_keyPress(Keyboard* keyboard, InputKey key, bool isPressed, void* userPtr)
{
	RenderPass_ReactiveKey* pass= (RenderPass_ReactiveKey*)userPtr;
	pass->OnKeyPressed(key, isPressed);
}

RenderPass_ReactiveKey::RenderPass_ReactiveKey(const float3& keyDownColor, float fadeOutTIme, const KeyboardState& triggerKey, RenderPass* underlyingPass)
	:RenderPass(underlyingPass)
{
	m_keyDownColor			= keyDownColor;
	m_fadeOutSpeed			= 1.0f/fadeOutTIme;
	m_triggerKey			= triggerKey;
	m_isUniqueFadeOutKey	= true;
	m_desireFrameRate		= 15.0f;
}

RenderPass_ReactiveKey::~RenderPass_ReactiveKey()
{
}
	
void RenderPass_ReactiveKey::bindKeyboard(Keyboard* keyboard)
{
	RenderPass::bindKeyboard(keyboard);
	keyboard->registerKeyPressCallback(&RenderPass_ReactiveKey_keyPress, this);
}
	
void RenderPass_ReactiveKey::unbindKeyboard(Keyboard* keyboard)
{
	RenderPass::unbindKeyboard(keyboard);
	keyboard->deregisterKeyPressCallback(&RenderPass_ReactiveKey_keyPress, this);
}

void RenderPass_ReactiveKey::updatePass(Keyboard* keyboard, float deltaSeconds)
{
	// update fade out key
	int		numFadingKey= m_fadeKey.num();
	float	speed		= m_fadeOutSpeed * deltaSeconds;
	for(int i=0; i<numFadingKey; ++i)
	{
		FadeOutKey& fadeOutKey= m_fadeKey[i];
		fadeOutKey.fadeOut-= speed;
		if (fadeOutKey.fadeOut <= 0.0f)
		{
			m_fadeKey.removeAt(i);
			--i;
			--numFadingKey;
		}
	}

	// update render pass state
	bool isTransparent= numFadingKey == 0 && m_downKey.num() == 0;
	unsigned int flag= m_flag;
	m_flag= isTransparent ? (flag | RENDER_PASS_FLAG_FULLY_TRANSPARENT) : (flag & ~RENDER_PASS_FLAG_FULLY_TRANSPARENT);
}

void RenderPass_ReactiveKey::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
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
	int		numFade		= m_fadeKey.num();
	for(int i=0; i<numFade; ++i)
	{
		FadeOutKey& fadeOutKey	= m_fadeKey[i];
		float		t			= fadeOutKey.fadeOut;
		int4 pos= layout->keyPixelPos[(int)fadeOutKey.key];
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
				float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
				float3 pressColor	= isBorder ? borderColor : downColor;
				float3 blendColor	= Math::lerp(originalColor, pressColor, t);
				outputRenderTarget->setPixel(x, y, blendColor);
			}
	}
}

void RenderPass_ReactiveKey::OnKeyPressed(InputKey key, bool isPressed)
{
	if (!m_triggerKey.isKeyOn(key))
		return;

	if (isPressed)
	{
		if (m_isUniqueFadeOutKey)
		{
			bool	isFadingKey	= false;
			int		numFade		= m_fadeKey.num();
			for(int i=0; i<numFade; ++i)
				if (m_fadeKey[i].key == key)
				{
					m_fadeKey.removeAt(i);
					break;
				}
		}
		m_downKey.addUnique(key);
	}
	else
	{
		if (m_downKey.remove(key))
			m_fadeKey.add({key, 1.0f});
	}
}
