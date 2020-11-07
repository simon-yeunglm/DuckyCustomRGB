
// by simon yeung, 18/10/2020
// all rights reserved

#include <memory>
#include <assert.h>
#include "RenderPass_ColorBreath.h"
#include "math/vector.h"
#include "math/Math.h"
#include "render/RenderTarget.h"

RenderPass_ColorBreath::RenderPass_ColorBreath(const float3* breathColor, int breathColorNum, float changeColorTime, RenderPass* underlyingPass)
	:RenderPass(underlyingPass)
{
	m_speed				= changeColorTime > 0 ? 1.0f / changeColorTime : 0;
	m_currentColorIdx	= 0;
	m_flag				|= RENDER_PASS_FLAG_FULLY_OPAQUE;
	m_timer= 0.0f;
	m_breathColorNum	= breathColorNum;
	size_t colorSz		= sizeof(float3)*breathColorNum;
	m_breathColor		= (float3*)malloc(colorSz);
	assert(m_breathColor!=nullptr);
	memcpy(m_breathColor, breathColor, colorSz);
	m_desireFrameRate	= 15.0f;
}

RenderPass_ColorBreath::~RenderPass_ColorBreath()
{
	free(m_breathColor);
}

void RenderPass_ColorBreath::updatePass(Keyboard* keyboard, float deltaSeconds)
{
	m_timer+= m_speed * deltaSeconds;
	if (m_timer >= 1.0f)
	{
		++m_currentColorIdx;
		if (m_currentColorIdx >= m_breathColorNum)
			m_currentColorIdx= 0;
		m_timer= 0.0f;
	}
}

void RenderPass_ColorBreath::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	int		idx0= m_currentColorIdx;
	int		idx1= (idx0 + 1) % m_breathColorNum;
	float3	col0= m_breathColor[idx0];
	float3	col1= m_breathColor[idx1];
	float	t	= Math::smoothStep(m_timer);
	float3	col = Math::lerp(col0, col1, t);
	float4	col4= { col.x, col.y, col.z, 1.0f };
	int2	rtSz= outputRenderTarget->size;
	for(int y= 0; y < rtSz.y; ++y)
		for(int x= 0; x < rtSz.x; ++x)
			outputRenderTarget->setPixel(x, y, col4);
}
