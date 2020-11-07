
// by simon yeung, 17/10/2020
// all rights reserved

#include "RenderTargetPool.h"
#include "RenderTarget.h"
#include <memory>
#include <assert.h>

RenderTargetPool::RenderTargetPool()
{
	m_size.set(0, 0);
}

RenderTargetPool::~RenderTargetPool()
{
	for(int i=0; i<m_freeList.num(); ++i)
		free(m_freeList[i]);
	
#ifdef _DEBUG
	assert(m_allocList.num() == 0);	// RT must be all free in a single render() called
#endif
}

void			RenderTargetPool::init(int2 size)
{
	m_size= size;
}

RenderTarget*	RenderTargetPool::allocRT()
{
	RenderTarget* renderTarget;
	if (m_freeList.num())
		renderTarget= m_freeList.removeLast();
	else
	{
		renderTarget		= (RenderTarget*)malloc(sizeof(RenderTarget) + sizeof(float4)*m_size.x*m_size.y);
		assert(renderTarget);
		renderTarget->size	= m_size;
	}
#ifdef _DEBUG
	m_allocList.add(renderTarget);
#endif
	return renderTarget;
}

void			RenderTargetPool::freeRT(RenderTarget* renderTarget)
{
#ifdef _DEBUG
	bool isFound= m_allocList.remove(renderTarget);
	assert(isFound);
#endif
	m_freeList.add(renderTarget);
}
