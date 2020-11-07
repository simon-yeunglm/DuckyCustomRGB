#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

#include "RenderTargetPool.h"
#include "RenderLayout.h"

class RenderPass;
class Keyboard;
struct RenderTarget;

class RenderGraph
{
private:
	RenderTargetPool	m_renderTargetPool;
	Keyboard*			m_keyboard;
	RenderLayout		m_layout;
	RenderPass*			m_pass;

	void	setKeyboardColor(RenderTarget* renderTarget);
		
	void	setLayoutTKL_ANSI();
	void	setLayoutFull_ANSI();
	
	void	addLayoutKeyTKL_ANSI();
	void	addLayoutKeyNumpad();

public:
	RenderGraph(Keyboard* keyboard);
	~RenderGraph();

	void	setGraph(RenderPass* pass);
	float	update(float deltaSeconds);
	void	render();
};
