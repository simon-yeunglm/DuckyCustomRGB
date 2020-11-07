#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

#include "math/vector.h"
#include "collection/Array.h"

struct RenderTarget;

class RenderTargetPool
{
private:
	int2					m_size;
	Array<RenderTarget*>	m_freeList;

#ifdef _DEBUG
	Array<RenderTarget*>	m_allocList;
#endif

public:
	RenderTargetPool();
	~RenderTargetPool();

	void			init(int2 size);

	RenderTarget*	allocRT();
	void			freeRT(RenderTarget* renderTarget);

};
