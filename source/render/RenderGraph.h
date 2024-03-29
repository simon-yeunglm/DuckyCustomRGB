#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

#include "RenderTargetPool.h"
#include "RenderLayout.h"

class RenderPass;
class Mouse;
class Keyboard;
struct RenderTarget;

class RenderGraph
{
private:
	RenderTargetPool	m_renderTargetPool;
	Keyboard*			m_keyboard;
	Mouse*				m_mouse;
	RenderLayout		m_layout;
	RenderPass*			m_pass;

	void	setKeyboardColor(	RenderTarget* renderTarget);
	void	setMouseColor(		RenderTarget* renderTarget);
		
	void	setLayoutTKL_ANSI();
	void	setLayoutFull_ANSI();
	void	setLayout_1800_ANSI();
	
	void	addLayoutKeyTKL_ANSI();
	void	addLayoutKeyNumpad();
	void	addLayoutKey_1800_ANSI();

public:
	RenderGraph(Keyboard* keyboard, Mouse* mouse);
	~RenderGraph();

	void	setGraph(RenderPass* pass);
	float	update(float deltaSeconds);
	void	render();
};
