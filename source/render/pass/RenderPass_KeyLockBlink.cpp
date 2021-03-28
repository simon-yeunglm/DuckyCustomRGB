
// by simon yeung, 25/10/2020
// all rights reserved

#include "RenderPass_KeyLockBlink.h"
#include "render/RenderLayout.h"
#include "render/RenderTarget.h"
#include "keyboard/Keyboard.h"
#include "math/Math.h"
#include "math/Color.h"
#include "win.h"

static void RenderPass_KeyLockBlink_keyPress(Keyboard* keyboard, InputKey key, bool isPressed, void* userPtr)
{
	RenderPass_KeyLockBlink* pass= (RenderPass_KeyLockBlink*)userPtr;
	pass->OnKeyPressed(key, isPressed);
}

RenderPass_KeyLockBlink::RenderPass_KeyLockBlink(float blinkSpeed, RenderPass* underlyingPass)
	:RenderPass(underlyingPass)
{
	m_BlinkSpeed		= blinkSpeed;
	m_desireFrameRate	= 12.0f;
	updateLockCaps();
	updateLockScroll();
//	updateLockNum();
}

RenderPass_KeyLockBlink::~RenderPass_KeyLockBlink()
{
}
	
void RenderPass_KeyLockBlink::bindKeyboard(Keyboard* keyboard)
{
	RenderPass::bindKeyboard(keyboard);
	keyboard->registerKeyPressCallback(&RenderPass_KeyLockBlink_keyPress, this);
}
	
void RenderPass_KeyLockBlink::unbindKeyboard(Keyboard* keyboard)
{
	RenderPass::unbindKeyboard(keyboard);
	keyboard->deregisterKeyPressCallback(&RenderPass_KeyLockBlink_keyPress, this);
}

void RenderPass_KeyLockBlink::updatePass(Keyboard* keyboard, float deltaSeconds)
{
	m_blinkTimer= Math::mod(m_blinkTimer + m_BlinkSpeed * deltaSeconds, 1.0f);

	// update render pass state
	bool isBlinking= m_isLockCaps || m_isLockScroll;// || m_isLockNum;
	unsigned int flag= m_flag;
	m_flag= isBlinking ? (flag & ~RENDER_PASS_FLAG_FULLY_TRANSPARENT) : (flag | RENDER_PASS_FLAG_FULLY_TRANSPARENT);
}

void RenderPass_KeyLockBlink::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	if (m_isLockCaps)
		renderBlink(InputKey::CapsLock	, layout, outputRenderTarget);
	if (m_isLockScroll)
		renderBlink(InputKey::ScrollLock	, layout, outputRenderTarget);
//	if (m_isLockNum)
//		renderBlink(InputKey::NumLock	, layout, outputRenderTarget);
}

void RenderPass_KeyLockBlink::renderBlink(InputKey key, const RenderLayout* layout, RenderTarget* outputRenderTarget)
{
	float blinkMultiplier= m_blinkTimer > 0.5f ?1.6f: 0.25f;
	int4 pos= layout->keyPixelPos[(int)key];
	for(int y=pos.y; y<pos.w; ++y)
		for(int x=pos.x; x<pos.z; ++x)
		{
			float3 originalColor= outputRenderTarget->getPixel(x, y).xyz();
			float3	HSV			= Color::RGBtoHSV(originalColor);
			float	V			= HSV.z >= 0.5f ? HSV.z : 1.0f - HSV.z;
			V					*= blinkMultiplier;
			float3 blinkColor	= Color::HSVtoRGB( { HSV.x, HSV.y, V } );
			blinkColor.x		= Math::clamp(blinkColor.x, 0.0f, 1.0f);
			blinkColor.y		= Math::clamp(blinkColor.y, 0.0f, 1.0f);
			blinkColor.z		= Math::clamp(blinkColor.z, 0.0f, 1.0f);
			outputRenderTarget->setPixel(x, y, blinkColor);
		}
}

void RenderPass_KeyLockBlink::OnKeyPressed(InputKey key, bool isPressed)
{
	if (		key== InputKey::CapsLock)
		updateLockCaps();
	else if (	key== InputKey::ScrollLock)
		updateLockScroll();
//	else if (	key== InputKey::NumLock)
//		updateLockNum();
}

void	RenderPass_KeyLockBlink::updateLockCaps()
{
	SHORT state= GetKeyState(VK_CAPITAL);
	m_isLockCaps= (state & 0x01)!= 0;
}

void	RenderPass_KeyLockBlink::updateLockScroll()
{
	SHORT state= GetKeyState(VK_SCROLL);
	m_isLockScroll= (state & 0x01)!= 0;
}

void	RenderPass_KeyLockBlink::updateLockNum()
{
	SHORT state= GetKeyState(VK_NUMLOCK);
	m_isLockNum= (state & 0x01)!= 0;
}
