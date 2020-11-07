#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

class RenderTargetPool;
class Keyboard;
struct RenderLayout;
struct RenderTarget;

#define RENDER_PASS_FLAG_FULLY_OPAQUE		(0x01)		// all pixels have alpha == 1
#define RENDER_PASS_FLAG_FULLY_TRANSPARENT	(0x02)		// all pixels have alpha == 0
#define RENDER_PASS_FLAG_ALWAYS_UPDATE		(0x04)		// will skip m_desireFrameRate and use underlying pass FPS if not always update

// a baisc render pass with 1 underlying layer to blend with
class RenderPass
{
protected:
	RenderPass*		m_underlyingPass;
	unsigned int	m_flag;
	float			m_desireFrameRate;
	
	virtual void bindKeyboard(Keyboard* keyboard);
	virtual void unbindKeyboard(Keyboard* keyboard);

	virtual void updatePass(Keyboard* keyboard, float deltaSeconds);
	virtual void renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget);

public:
	RenderPass(RenderPass* underlyingPass= nullptr);
	virtual ~RenderPass();

	// return desire framerate
	virtual float	update(	Keyboard* keyboard, float deltaSeconds);
	virtual void	render(	const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget);

	inline bool isFullyOpaque()const		{ return (m_flag & RENDER_PASS_FLAG_FULLY_OPAQUE		) != 0; }
	inline bool isFullyTransparent()const	{ return (m_flag & RENDER_PASS_FLAG_FULLY_TRANSPARENT	) != 0; }

	friend class RenderGraph;
};

