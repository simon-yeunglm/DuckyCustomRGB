
// by simon yeung, 26/10/2020
// all rights reserved

#include "RenderPass_Idle.h"
#include "render/RenderTarget.h"
#include "keyboard/Keyboard.h"
#include "math/Math.h"
#include "math/Color.h"

static void RenderPass_Idle_keyPress(Keyboard* keyboard, KeyboardKey key, bool isPressed, void* userPtr)
{
	RenderPass_Idle* pass= (RenderPass_Idle*)userPtr;
	pass->OnKeyPressed(key, isPressed);
}

RenderPass_Idle::RenderPass_Idle(float idleTime, RenderPass* underlyingPass)
	: RenderPass(underlyingPass)
{
	m_state					= State::WakingUpFromIdle;
	m_stateTimer			= 0.0f;
	m_idleTime				= idleTime;
	m_idleTimer				= 0.0f;
	m_wakeUpSlashTime		= 0.7f;
	m_wakeUpShowVertLineTime= 0.2f;
	m_wakeUpMoveVertLineTime= 0.45f;
	m_enterIdleTime			= 0.25f;
}

RenderPass_Idle::~RenderPass_Idle()
{
}

void RenderPass_Idle::bindKeyboard(  Keyboard* keyboard)
{
	RenderPass::bindKeyboard(keyboard);
	keyboard->registerKeyPressCallback(&RenderPass_Idle_keyPress, this);
}

void RenderPass_Idle::unbindKeyboard(Keyboard* keyboard)
{
	RenderPass::unbindKeyboard(keyboard);
	keyboard->deregisterKeyPressCallback(&RenderPass_Idle_keyPress, this);
}

float RenderPass_Idle::update(	Keyboard* keyboard, float deltaSeconds)
{
	updatePass(keyboard, deltaSeconds);
	float underlyingPassFPS= 0.0f;
	if (m_underlyingPass && m_state	!= State::Idle)
		underlyingPassFPS= m_underlyingPass->update(keyboard, deltaSeconds);
	
	if (m_state == State::Idle)
		return 0.0f;
	else if (m_state == State::Awake)
		return underlyingPassFPS;
	else
		return Math::max(underlyingPassFPS, 30.0f);
}

void RenderPass_Idle::updatePass(Keyboard* keyboard, float deltaSeconds)
{
	// update idle timer
	static_assert((int)State::Num == 5, "Add new State case.");
	if (m_state == State::Idle)
	{
		// nothing to do
	}
	else if (m_state == State::WakingUpFromIdle)
		updateWakingUpFromIdle(deltaSeconds);
	else if (m_state == State::WakingUpFromEnteringIdle)
		updateWakingUpFromEnteringIdle(deltaSeconds);
	else if (m_state == State::Awake)
		updateAwake(deltaSeconds);
	else if (m_state == State::EnteringIdle)
		updateEnteringIdle(deltaSeconds);
	else
		assert(false);

	// update render pass state
	unsigned int flag= m_flag;
	if (m_state	== State::Idle)
	{
		flag&=	~RENDER_PASS_FLAG_FULLY_TRANSPARENT;
		flag|=	 RENDER_PASS_FLAG_FULLY_OPAQUE;
		m_flag= flag ;
	}
	else if (m_state	== State::Awake)
	{
		flag&=	~RENDER_PASS_FLAG_FULLY_OPAQUE;
		flag|=	 RENDER_PASS_FLAG_FULLY_TRANSPARENT;
		m_flag= flag ;
	}
	else
		m_flag= flag & ~(RENDER_PASS_FLAG_FULLY_OPAQUE|RENDER_PASS_FLAG_FULLY_TRANSPARENT);
}

void RenderPass_Idle::updateWakingUpFromIdle(			float deltaSeconds)
{
	m_stateTimer+= Math::min(deltaSeconds, 1.0f/20.0f);	// clamp to avoid framerate spike from idle
	if (m_stateTimer >= (m_wakeUpSlashTime + m_wakeUpShowVertLineTime + m_wakeUpMoveVertLineTime))
		changeState(State::Awake);
}

void RenderPass_Idle::updateWakingUpFromEnteringIdle(	float deltaSeconds)
{
	m_stateTimer-= deltaSeconds;
	if (m_stateTimer <= 0.0f)
		changeState(State::Awake);
}

void RenderPass_Idle::updateAwake(					float deltaSeconds)
{
	float prevTimer	= m_idleTimer;
	float currTimer	= prevTimer + deltaSeconds;
	m_idleTimer		= currTimer;
	if (currTimer >= m_idleTime && prevTimer< m_idleTime)
		changeState(State::EnteringIdle);
}

void RenderPass_Idle::updateEnteringIdle(				float deltaSeconds)
{
	m_stateTimer+= deltaSeconds;
	if (m_stateTimer >= m_enterIdleTime)
		changeState(State::Idle);
}

void RenderPass_Idle::renderPass(const RenderLayout* layout, RenderTargetPool* renderTargetPool, RenderTarget* outputRenderTarget)
{
	static_assert((int)State::Num == 5, "Add new State case.");
	if (m_state == State::Idle)
		renderIdle(outputRenderTarget);
	else if (m_state == State::WakingUpFromIdle)
		renderWakingUpFromIdle(outputRenderTarget);
	else if (m_state == State::WakingUpFromEnteringIdle)
		renderEnteringIdle(outputRenderTarget);
	else if (m_state == State::Awake)
	{
		// nothing to do, let underlying layers to render
	}
	else if (m_state == State::EnteringIdle)
		renderEnteringIdle(outputRenderTarget);
	else
		assert(false);
}

void	RenderPass_Idle::renderIdle(				RenderTarget* outputRenderTarget)
{
	// render black to block underlying layers
	float4	black	= { 0.0f, 0.0f, 0.0f, 1.0f };
	int2	rtSz	= outputRenderTarget->size;
	for(int y= 0; y < rtSz.y; ++y)
		for(int x= 0; x < rtSz.x; ++x)
			outputRenderTarget->setPixel(x, y, black);
}

void	RenderPass_Idle::renderWakingUpFromIdle(	RenderTarget* outputRenderTarget)
{
	float4		black		= { 0.0f, 0.0f, 0.0f, 1.0f };
	int2		rtSz		= outputRenderTarget->size;
	int			rtHalfW		= rtSz.x / 2;
	int			rtHalfH		= rtSz.y/ 2;
	const float	initVertLineHalfWidth	= 4.0f;
	if (m_stateTimer < m_wakeUpSlashTime)
	{
		float		t				= m_stateTimer / m_wakeUpSlashTime;
		float		m				= 4.0f;							// line - slope
		float		c				= Math::lerp(70.0f, -350.0f, t);// line - intercept
		float		a				= m;							// line equation general form
		float		b				=-1.0f;							// line equation general form
		float		disToLineFactor	= 1.0f/Math::sqrt(a*a+b*b);		// denominator of distance to line equation
		const float	lineHalfWidth	= 10.5f;
	
		for(int y= 0; y < rtSz.y; ++y)
			for(int x= 0; x < rtSz.x; ++x)
			{
				float dis	= Math::abs(a*x + b*y + c) * disToLineFactor;
				float alpha	= Math::min(dis/lineHalfWidth, 1.0f);
				alpha		*= alpha;

				float3 originalColor	= outputRenderTarget->getPixel(x, y).xyz();
				originalColor= Color::RGBtoHSV(originalColor);
				originalColor.y*= 1.3f;
				originalColor.z*= 1.5f;
				originalColor= Color::HSVtoRGB(originalColor);
				originalColor.x= Math::clamp(originalColor.x, 0.0f, 1.0f);
				originalColor.y= Math::clamp(originalColor.y, 0.0f, 1.0f);
				originalColor.z= Math::clamp(originalColor.z, 0.0f, 1.0f);
				float3 blendColor		= Math::lerp(originalColor, black.xyz(), alpha);
				outputRenderTarget->setPixel(x, y, blendColor);
			}
	} 
	else if (m_stateTimer < (m_wakeUpSlashTime + m_wakeUpShowVertLineTime))
	{
		float t				= (m_stateTimer - m_wakeUpSlashTime)/ m_wakeUpShowVertLineTime;
		for(int y= 0; y < rtSz.y; ++y)
			for(int x= 0; x < rtSz.x; ++x)
				outputRenderTarget->setPixel(x, y, black);
	}
	else
	{
		float t	= (m_stateTimer - (m_wakeUpSlashTime + m_wakeUpShowVertLineTime))/ m_wakeUpMoveVertLineTime;
		t		= 1.0f - t;

		float	blackWidthF= Math::lerp(initVertLineHalfWidth, rtSz.x * 0.5f, t);
		int		blackWidth	= (int)blackWidthF;
		float	subPxT		= Math::mod(blackWidthF, 1.0f);

		// left black line
		for(int x= 0; x < blackWidth	; ++x)
			for(int y= 0; y < rtSz.y; ++y)
				outputRenderTarget->setPixel(x, y, black);
		int upperBoundRow= blackWidth;
		for(int y= 0; y < rtSz.y; ++y)
		{
			float3 originalColor	= outputRenderTarget->getPixel(upperBoundRow, y).xyz();
			float3 blendColor		= Math::lerp(originalColor, black.xyz(), subPxT);
			outputRenderTarget->setPixel(upperBoundRow, y, blendColor);
		}
	
		// right lower line
		for(int x= rtSz.x - blackWidth; x < rtSz.x; ++x)
			for(int y= 0; y < rtSz.y; ++y)
				outputRenderTarget->setPixel(x, y, black);
		int lowerBoundRow= rtSz.x - blackWidth - 1;
		for(int y= 0; y < rtSz.y; ++y)
		{
			float3 originalColor	= outputRenderTarget->getPixel(lowerBoundRow, y).xyz();
			float3 blendColor		= Math::lerp(originalColor, black.xyz(), subPxT);
			outputRenderTarget->setPixel(lowerBoundRow, y, blendColor);
		}
	}
}

void	RenderPass_Idle::renderEnteringIdle(		RenderTarget* outputRenderTarget)
{
	float	t			= m_stateTimer / m_enterIdleTime;
	float4	black		= { 0.0f, 0.0f, 0.0f, 1.0f };
	int2	rtSz		= outputRenderTarget->size;
	float	blackHeightF= rtSz.y * (0.5f * t);
	int		blackHeight	= (int)blackHeightF;
	float	subPxT		= Math::mod(blackHeightF, 1.0f);

	
	// upper black line
	for(int y= 0; y < blackHeight	; ++y)
		for(int x= 0; x < rtSz.x; ++x)
			outputRenderTarget->setPixel(x, y, black);
	int upperBoundRow= blackHeight;
	for(int x= 0; x < rtSz.x; ++x)
	{
		float3 originalColor	= outputRenderTarget->getPixel(x, upperBoundRow).xyz();
		float3 blendColor		= Math::lerp(originalColor, black.xyz(), subPxT);
		outputRenderTarget->setPixel(x, upperBoundRow, blendColor);
	}
	
	// upper lower line
	for(int y= rtSz.y - blackHeight; y < rtSz.y; ++y)
		for(int x= 0; x < rtSz.x; ++x)
			outputRenderTarget->setPixel(x, y, black);
	int lowerBoundRow= rtSz.y - blackHeight - 1;
	for(int x= 0; x < rtSz.x; ++x)
	{
		float3 originalColor	= outputRenderTarget->getPixel(x, lowerBoundRow).xyz();
		float3 blendColor		= Math::lerp(originalColor, black.xyz(), subPxT);
		outputRenderTarget->setPixel(x, lowerBoundRow, blendColor);
	}
}

void	RenderPass_Idle::changeState(State newState)
{
	if (!(m_state == State::EnteringIdle && newState == State::WakingUpFromEnteringIdle))
		m_stateTimer= 0.0f;
	m_state = newState;
}

void RenderPass_Idle::OnKeyPressed(KeyboardKey key, bool isPressed)
{
	if (m_state == State::Idle)
		changeState(State::WakingUpFromIdle);
	else if (m_state == State::EnteringIdle)
		changeState(State::WakingUpFromEnteringIdle);

	// reset idle state
	m_idleTimer= 0;
}
