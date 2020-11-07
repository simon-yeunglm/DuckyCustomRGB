#pragma once

// by simon yeung, 25/10/2020
// all rights reserved

#include "../RenderPass.h"

struct KeyboardState;

// blend the underlaying layers based on which key is pressed
class RenderPass_KeyStateBlend : public RenderPass
{
protected:
	RenderPass**	m_keyStatePasses;
	KeyboardState*	m_keyStates;
	float*			m_weights;
	int				m_numKeyStates;
	float			m_blendSpeed;
	
	virtual float	update(	Keyboard* keyboard, float deltaSeconds)override;
	virtual void	render(	const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)override;
	
private:
	void		init(float blendTime, int numKeyStates, const KeyboardState* keyStates, const RenderPass** keyStatePasses);

public:
	// TODO: make variable argument
	RenderPass_KeyStateBlend(float blendTime, RenderPass* noKeyPressPass, KeyboardState keyState1, RenderPass* keyStatePasses1, KeyboardState keyState2, RenderPass* keyStatePasses2, KeyboardState keyState3, RenderPass* keyStatePasses3);
	RenderPass_KeyStateBlend(float blendTime, RenderPass* noKeyPressPass, int numKeyStates, const KeyboardState* keyStates, const RenderPass** keyStatePasses);
	virtual ~RenderPass_KeyStateBlend();
};

