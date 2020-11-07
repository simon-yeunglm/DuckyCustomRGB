
// by simon yeung, 17/10/2020
// all rights reserved

#include "RenderPass.h"
#include "math/Math.h"

RenderPass::RenderPass(RenderPass* underlyingPass)
{
	m_underlyingPass	= underlyingPass;
	m_flag				= 0;;
	m_desireFrameRate	= 0.0f;
}

RenderPass::~RenderPass()
{
	if (m_underlyingPass)
		delete m_underlyingPass;
}

void RenderPass::bindKeyboard(Keyboard* keyboard)
{
	if (m_underlyingPass)
		m_underlyingPass->bindKeyboard(keyboard);
}

void RenderPass::unbindKeyboard(Keyboard* keyboard)
{
	if (m_underlyingPass)
		m_underlyingPass->unbindKeyboard(keyboard);
}

float RenderPass::update(Keyboard* keyboard, float deltaSeconds)
{
	updatePass(keyboard, deltaSeconds);
	float underlyingPassFPS= 0.0f;
	if (m_underlyingPass)
		underlyingPassFPS= m_underlyingPass->update(keyboard, deltaSeconds);
	unsigned int flag= m_flag;
	if ((flag & RENDER_PASS_FLAG_ALWAYS_UPDATE		) == 0 &&
		(flag & RENDER_PASS_FLAG_FULLY_TRANSPARENT	) == 0 )
		return Math::max(underlyingPassFPS, m_desireFrameRate);
	else
		return underlyingPassFPS;
}

void RenderPass::updatePass(Keyboard* keyboard, float deltaSeconds)
{
}

void RenderPass::render(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	if (!isFullyOpaque() && m_underlyingPass)
		m_underlyingPass->render(layout, renderTargetPool, outputRenderTarget);
	
	if (!isFullyTransparent())
		renderPass(layout, renderTargetPool, outputRenderTarget);
}

void RenderPass::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
}
