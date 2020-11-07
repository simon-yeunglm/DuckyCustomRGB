
// by simon yeung, 18/10/2020
// all rights reserved

#include <memory>
#include <assert.h>
#include "RenderPass_ColorScroll.h"
#include "math/vector.h"
#include "math/Math.h"
#include "../RenderTarget.h"

RenderPass_ColorScroll::RenderPass_ColorScroll(const float3* scrollColor, int scrollColorNum, float distanceBetweenColor, float changeColorTime, RenderPass* underlyingPass)
	:RenderPass(underlyingPass)
{
	m_speed				= changeColorTime > 0 ? 1.0f / changeColorTime : 0;
	m_currentColorIdx	= 0;
	m_flag				|= RENDER_PASS_FLAG_FULLY_OPAQUE;
	m_timer				= 0.0f;
	m_indexOffset		= 1.0f/(distanceBetweenColor * NUM_PIXEL_PER_KEY);
	m_scrollColorNum	= scrollColorNum;
	size_t colorSz		= sizeof(float3)*scrollColorNum;
	m_scrollColor		= (float3*)malloc(colorSz);
	assert(m_scrollColor != nullptr);
	memcpy(m_scrollColor, scrollColor, colorSz);
	m_desireFrameRate	= 15.0f;
}

RenderPass_ColorScroll::~RenderPass_ColorScroll()
{
	free(m_scrollColor);
}

void RenderPass_ColorScroll::updatePass(Keyboard* keyboard, float deltaSeconds)
{
	m_timer -= m_speed * deltaSeconds;
	if (Math::abs(m_timer) >= m_scrollColorNum)
		m_timer= 0.0f;
}

void RenderPass_ColorScroll::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	const float3* color	= m_scrollColor;
	float	idxOffset	= m_indexOffset;
	int		numColor	= m_scrollColorNum;
	int2	rtSz		= outputRenderTarget->size;
	float	idxStart	= numColor + m_timer;
	for(int x= 0; x < rtSz.x; ++x)
	{
		float	idxT= Math::mod(idxStart + x * idxOffset, (float)numColor);
		int		idx0= (int)idxT;
		int		idx1= (idx0 + 1) % numColor;
		float3	col0= color[idx0];
		float3	col1= color[idx1];

		float	t	= idxT - (float)idx0;
		float3	col = Math::lerp(col0, col1, t);

		float4	col4= { col.x, col.y, col.z, 1.0f };
		for(int y= 0; y < rtSz.y; ++y)
			outputRenderTarget->setPixel(x, y, col4);
	}
}
