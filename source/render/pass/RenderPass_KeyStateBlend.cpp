#pragma once

// by simon yeung, 25/10/2020
// all rights reserved

#include "RenderPass_KeyStateBlend.h"
#include "keyboard/KeyboardState.h"
#include "keyboard/Keyboard.h"
#include "math/Math.h"
#include "render/RenderTarget.h"
#include "render/RenderTargetPool.h"
#include <assert.h>
#include <memory>

RenderPass_KeyStateBlend::RenderPass_KeyStateBlend(float blendTime, RenderPass* noKeyPressPass, KeyboardState keyState1, RenderPass* keyStatePasses1, KeyboardState keyState2, RenderPass* keyStatePasses2, KeyboardState keyState3, RenderPass* keyStatePasses3)
	:RenderPass(noKeyPressPass)
{
	const RenderPass*	keyStatePasses[]= { keyStatePasses1	, keyStatePasses2	, keyStatePasses3 };
	KeyboardState		keyStates[]		= { keyState1		, keyState2			, keyState3 };
	init(blendTime, 3, keyStates, keyStatePasses);
}

RenderPass_KeyStateBlend::RenderPass_KeyStateBlend(float blendTime, RenderPass* noKeyPressPass, int numKeyStates, const KeyboardState* keyStates, const RenderPass** keyStatePasses)
	:RenderPass(noKeyPressPass)
{
	init(blendTime, numKeyStates, keyStates, keyStatePasses);
}

void	RenderPass_KeyStateBlend::init(float blendTime, int numKeyStates, const KeyboardState* keyStates, const RenderPass** keyStatePasses)
{
	assert(numKeyStates > 0);
	m_blendSpeed	= 1.0f/blendTime;
	m_numKeyStates	= numKeyStates;
	int numWeight	= numKeyStates + 1;
	int sz_pass		= sizeof(RenderPass*	) * numWeight;
	int sz_state	= sizeof(KeyboardState	) * numKeyStates;
	int sz_weight	= sizeof(float			) * numWeight;
	m_keyStatePasses= (RenderPass**			)malloc(sz_pass);
	m_keyStates		= (KeyboardState*		)malloc(sz_state);
	m_weights		= (float*				)malloc(sz_weight);
	assert(m_keyStatePasses	);
	assert(m_keyStates		);
	assert(m_weights		);
	m_keyStatePasses[0]= m_underlyingPass;
	memcpy(m_keyStatePasses	+ 1	, keyStatePasses , sizeof(RenderPass*) *	numKeyStates);
	memcpy(m_keyStates			, keyStates		, sz_state	);

	m_weights[0] = 1.0f;
	for(int i=0; i<numWeight; ++i)
		m_weights[i]= 0.0f;
}

RenderPass_KeyStateBlend::~RenderPass_KeyStateBlend()
{
	for(int i=1; i<=m_numKeyStates; ++i)
		delete m_keyStatePasses[i];
	free(m_keyStatePasses);
	free(m_keyStates);
	free(m_weights);
}

float RenderPass_KeyStateBlend::update(	Keyboard* keyboard, float deltaSeconds)
{
	// find active layer
	int numKeyStates= m_numKeyStates;
	int activeLayerIdx= 0;
	for(int i=0; i<numKeyStates; ++i)
		if (keyboard && keyboard->isAnyKeyDown(m_keyStates[i]))
		{
			activeLayerIdx= i + 1;
			break;
		}

	// update layer weight
	float blendSpeed= m_blendSpeed;
	float fadeInSpeed	= deltaSeconds * blendSpeed;
	float fadeOutSpeed	=-deltaSeconds * blendSpeed;
	for(int i=0; i<=numKeyStates; ++i)
	{
		float deltaWeight	= i == activeLayerIdx ? fadeInSpeed : fadeOutSpeed;
		m_weights[i]		= Math::clamp(m_weights[i] + deltaWeight, 0.0f, 1.0f);
	}

	// update underlying layer
	float frameRate= 0.0f;
	for(int i=0; i<=numKeyStates; ++i)
		frameRate= Math::max(m_keyStatePasses[i]->update(keyboard, deltaSeconds), frameRate);
	return frameRate;
}

void RenderPass_KeyStateBlend::render(	const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	// calculate weight
	int		numActiveLayer	= 0;
	float	totalWeight		= 0.0f;
	int		numKeyStates	= m_numKeyStates;
	int		activeLayerIdx	= -1;
	for(int i=0; i<=numKeyStates; ++i)
	{
		float weight	= m_weights[i];
		bool isActive	= weight > 0;
		numActiveLayer	+= isActive;
		totalWeight		+= weight;
		if (isActive)
			activeLayerIdx= i;
	}

	// fast path for only 1 active layer
	if (numActiveLayer == 1)
	{
		// only single layer, no need blending
		assert(activeLayerIdx != -1);
		m_keyStatePasses[activeLayerIdx]->render(layout, renderTargetPool, outputRenderTarget);
	}
	else
	{
		// blend multiple layers
		int2	rtSz	= outputRenderTarget->size;
		float4	black	= { 0.0f, 0.0f, 0.0f, 0.0f };
		for(int y= 0; y < rtSz.y; ++y)
			for(int x= 0; x < rtSz.x; ++x)
				outputRenderTarget->setPixel(x, y, black);

		// iterate all active layer
		RenderTarget* RT= renderTargetPool->allocRT();
		for(int i=0; i<=numKeyStates; ++i)
		{
			float weight	= m_weights[i];
			if (weight <= 0.0f)
				continue;
			weight/= totalWeight;
			m_keyStatePasses[i]->render(layout, renderTargetPool, RT);
			
			// accumulate pixel color
			for(int y= 0; y < rtSz.y; ++y)
				for(int x= 0; x < rtSz.x; ++x)
				{
					float3 accumulate= outputRenderTarget->getPixel(x, y).xyz();
					accumulate+= RT->getPixel(x, y).xyz() * weight;
					outputRenderTarget->setPixel(x, y, accumulate);
				}
		}
		renderTargetPool->freeRT(RT);
	}
}
