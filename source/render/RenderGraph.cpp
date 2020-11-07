
// by simon yeung, 17/10/2020
// all rights reserved

#include "RenderGraph.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "math/Math.h"
#include "keyboard/Keyboard.h"
#include <assert.h>

RenderGraph::RenderGraph(Keyboard* keyboard)
{
	m_pass					= nullptr;
	m_keyboard				= keyboard;
	KeyboardLayout layout	= keyboard->getLayout();
	if (layout == KeyboardLayout::TKL_ANSI)
		setLayoutTKL_ANSI();
	else if (layout == KeyboardLayout::Full_ANSI)
		setLayoutFull_ANSI();
	else
	{
		assert(false);	// unsupport layout
		memset(&m_layout, 0, sizeof(m_layout));
	}
}

RenderGraph::~RenderGraph()
{
	if (m_pass)
		delete m_pass;
}

void	RenderGraph::setLayoutTKL_ANSI()
{
	memset(&m_layout, 0, sizeof(m_layout));
	addLayoutKeyTKL_ANSI();
	m_renderTargetPool.init({72, 24});
}

void	RenderGraph::setLayoutFull_ANSI()
{
	memset(&m_layout, 0, sizeof(m_layout));
	addLayoutKeyTKL_ANSI();
	addLayoutKeyNumpad();
	m_renderTargetPool.init({88, 24});
}

void	RenderGraph::addLayoutKeyTKL_ANSI()
{
	m_layout.keyPixelPos[(int)KeyboardKey::A					]= {  7, 12, 11, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::B					]= { 25, 16, 29, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::C					]= { 17, 16, 21, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::D					]= { 15, 12, 19, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::E					]= { 14,  8, 18, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::F					]= { 19, 12, 23, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::G					]= { 23, 12, 27, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::H					]= { 27, 12, 31, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::I					]= { 34,  8, 38, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::J					]= { 31, 12, 35, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::K					]= { 35, 12, 39, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::L					]= { 39, 12, 43, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::M					]= { 33, 16, 37, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::N					]= { 29, 16, 33, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::O					]= { 38,  8, 42, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::P					]= { 42,  8, 46, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::Q					]= {  6,  8, 10, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::R					]= { 18,  8, 22, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::S					]= { 11, 12, 15, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::T					]= { 22,  8, 26, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::U					]= { 30,  8, 34, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::V					]= { 21, 16, 25, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::W					]= { 10,  8, 14, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::X					]= { 13, 16, 17, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::Y					]= { 26,  8, 30, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::Z					]= {  9, 16, 13, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_0				]= { 40,  4, 44,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_1				]= {  4,  4,  8,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_2				]= {  8,  4, 12,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_3				]= { 12,  4, 16,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_4				]= { 16,  4, 20,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_5				]= { 20,  4, 24,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_6				]= { 24,  4, 28,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_7				]= { 28,  4, 32,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_8				]= { 32,  4, 36,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Num_9				]= { 36,  4, 40,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Escape				]= {  0,  0,  4,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::Tilde				]= {  0,  4,  4,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Plus					]= { 48,  4, 52,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Minus				]= { 44,  4, 48,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::BackSpace			]= { 52,  4, 60,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Tab					]= {  0,  8,  6, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::Bracket_Open			]= { 46,  8, 50, 12 }; 
	m_layout.keyPixelPos[(int)KeyboardKey::Bracket_Close		]= { 50,  8, 54, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::Backslash			]= { 54,  8, 60, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::CapsLock				]= {  0, 12,  7, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::Colon				]= { 43, 12, 47, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::Apostrophe			]= { 47, 12, 51, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::Enter				]= { 51, 12, 60, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::Shift_Left			]= {  0, 16,  9, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::Shift_Right			]= { 49, 16, 60, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::Comma				]= { 37, 16, 41, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::Dot					]= { 41, 16, 45, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::ForwardSlash			]= { 45, 16, 49, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::SpaceBar				]= { 15, 20, 40, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Fn					]= { 50, 20, 55, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Control_Left			]= {  0, 20,  5, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Control_Right		]= { 55, 20, 60, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Windows_Left			]= {  5, 20, 10, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Windows_Right		]= { 45, 20, 50, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Alt_Left				]= { 10, 20, 15, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Alt_Right			]= { 40, 20, 45, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Arrow_Up				]= { 64, 16, 68, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::Arrow_Down			]= { 64, 20, 68, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Arrow_Left			]= { 60, 20, 64, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Arrow_Right			]= { 68, 20, 72, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::Insert				]= { 60,  4, 64,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::Delete				]= { 60,  8, 64, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::Home					]= { 64,  4, 68,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::End					]= { 64,  8, 68, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::PageUp				]= { 68,  4, 72,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::PageDown				]= { 68,  8, 72, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::PrintScreen			]= { 60,  0, 64,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::ScrollLock			]= { 64,  0, 68,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::Pause				]= { 68,  0, 72,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F1					]= {  8,  0, 12,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F2					]= { 12,  0, 16,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F3					]= { 16,  0, 20,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F4					]= { 20,  0, 24,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F5					]= { 26,  0, 30,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F6					]= { 30,  0, 34,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F7					]= { 34,  0, 38,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F8					]= { 38,  0, 42,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F9					]= { 44,  0, 48,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F10					]= { 48,  0, 52,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F11					]= { 52,  0, 56,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::F12					]= { 56,  0, 60,  4 };
}

void	RenderGraph::addLayoutKeyNumpad()
{
	m_layout.keyPixelPos[(int)KeyboardKey::NumLock				]= { 72,  4, 76,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_0				]= { 72, 20, 80, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_1				]= { 72, 16, 76, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_2				]= { 76, 16, 80, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_3				]= { 80, 16, 84, 20 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_4				]= { 72, 12, 76, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_5				]= { 76, 12, 80, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_6				]= { 80, 12, 84, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_7				]= { 72,  8, 76, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_8				]= { 76,  8, 80, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_9				]= { 80,  8, 84, 12 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Plus			]= { 84,  8, 88, 16 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Minus			]= { 84,  4, 88,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Multiply		]= { 80,  4, 84,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Divide		]= { 76,  4, 80,  8 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Dot			]= { 80, 20, 84, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Enter			]= { 84, 16, 88, 24 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Extra1		]= { 72,  0, 76,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Extra2		]= { 76,  0, 80,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Extra3		]= { 80,  0, 84,  4 };
	m_layout.keyPixelPos[(int)KeyboardKey::NumPad_Extra4		]= { 84,  0, 88,  4 };
}

void	RenderGraph::setGraph(RenderPass* pass)
{
	m_pass= pass;
	pass->bindKeyboard(m_keyboard);
}

float	RenderGraph::update(float deltaSeconds)
{
	return m_pass->update(m_keyboard, deltaSeconds);
}

void	RenderGraph::render()
{
	assert(m_pass != nullptr && m_keyboard != nullptr);
	RenderTarget* renderTarget= m_renderTargetPool.allocRT();
	m_pass->render(&m_layout, &m_renderTargetPool, renderTarget);
	setKeyboardColor(renderTarget);
	m_renderTargetPool.freeRT(renderTarget);
}

void	RenderGraph::setKeyboardColor(RenderTarget* renderTarget)
{
	for(int i=0; i<(int)KeyboardKey::Num; ++i)
	{
		int4	pxRange		= m_layout.keyPixelPos[i];
		int		numPx		= (pxRange.z - pxRange.x) * (pxRange.w - pxRange.y);
		if (numPx <= 0)
			continue;

		float3	averageVal	= { 0, 0, 0 };
		for(int x= pxRange.x; x<pxRange.z; ++x)
			for(int y= pxRange.y; y<pxRange.w; ++y)
			{
				float4 pxColor= renderTarget->getPixel(x, y);
				averageVal+= pxColor.xyz();
			}
		averageVal/= (float)numPx;
		
		unsigned char R= (unsigned char)(averageVal.x * 255);
		unsigned char G= (unsigned char)(averageVal.y * 255);
		unsigned char B= (unsigned char)(averageVal.z * 255);
		m_keyboard->setKeyColor((KeyboardKey)i, R, G, B);
	}
	m_keyboard->commitKeyColor();
}
