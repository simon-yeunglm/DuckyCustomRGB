
// by simon yeung, 17/10/2020
// all rights reserved

#include "RenderGraph.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "math/Math.h"
#include "keyboard/Keyboard.h"
#include "mouse/Mouse.h"
#include <assert.h>

#define MOUSE_PX_WIDTH		(4)

RenderGraph::RenderGraph(Keyboard* keyboard, Mouse* mouse)
{
	m_pass					= nullptr;
	m_mouse					= mouse;
	m_keyboard				= keyboard;
	if (keyboard)
	{
		KeyboardLayout layout	= keyboard->getLayout();
		if (layout == KeyboardLayout::TKL_ANSI)
			setLayoutTKL_ANSI();
		else if (layout == KeyboardLayout::Full_ANSI)
			setLayoutFull_ANSI();
		else if (layout == KeyboardLayout::_1800_ANSI)
			setLayout_1800_ANSI();
		else
		{
			assert(false);	// unsupport layout
			memset(&m_layout, 0, sizeof(m_layout));
		}
	}
	else
		memset(&m_layout, 0, sizeof(m_layout));
}

RenderGraph::~RenderGraph()
{
	if (m_pass)
		delete m_pass;
}

void	RenderGraph::setLayoutTKL_ANSI()
{
	bool hasMouse= m_mouse != nullptr;
	memset(&m_layout, 0, sizeof(m_layout));
	addLayoutKeyTKL_ANSI();
	m_renderTargetPool.init({72 + MOUSE_PX_WIDTH * hasMouse, 24});
}

void	RenderGraph::setLayoutFull_ANSI()
{
	bool hasMouse= m_mouse != nullptr;
	memset(&m_layout, 0, sizeof(m_layout));
	addLayoutKeyTKL_ANSI();
	addLayoutKeyNumpad();
	m_renderTargetPool.init({88 + MOUSE_PX_WIDTH * hasMouse, 24});
}

void	RenderGraph::setLayout_1800_ANSI()
{
	bool hasMouse= m_mouse != nullptr;
	memset(&m_layout, 0, sizeof(m_layout));
	addLayoutKey_1800_ANSI();
	m_renderTargetPool.init({76 + MOUSE_PX_WIDTH * hasMouse, 24});
}

void	RenderGraph::addLayoutKeyTKL_ANSI()
{
	m_layout.keyPixelPos[(int)InputKey::A					]= {  7, 12, 11, 16 };
	m_layout.keyPixelPos[(int)InputKey::B					]= { 25, 16, 29, 20 };
	m_layout.keyPixelPos[(int)InputKey::C					]= { 17, 16, 21, 20 };
	m_layout.keyPixelPos[(int)InputKey::D					]= { 15, 12, 19, 16 };
	m_layout.keyPixelPos[(int)InputKey::E					]= { 14,  8, 18, 12 };
	m_layout.keyPixelPos[(int)InputKey::F					]= { 19, 12, 23, 16 };
	m_layout.keyPixelPos[(int)InputKey::G					]= { 23, 12, 27, 16 };
	m_layout.keyPixelPos[(int)InputKey::H					]= { 27, 12, 31, 16 };
	m_layout.keyPixelPos[(int)InputKey::I					]= { 34,  8, 38, 12 };
	m_layout.keyPixelPos[(int)InputKey::J					]= { 31, 12, 35, 16 };
	m_layout.keyPixelPos[(int)InputKey::K					]= { 35, 12, 39, 16 };
	m_layout.keyPixelPos[(int)InputKey::L					]= { 39, 12, 43, 16 };
	m_layout.keyPixelPos[(int)InputKey::M					]= { 33, 16, 37, 20 };
	m_layout.keyPixelPos[(int)InputKey::N					]= { 29, 16, 33, 20 };
	m_layout.keyPixelPos[(int)InputKey::O					]= { 38,  8, 42, 12 };
	m_layout.keyPixelPos[(int)InputKey::P					]= { 42,  8, 46, 12 };
	m_layout.keyPixelPos[(int)InputKey::Q					]= {  6,  8, 10, 12 };
	m_layout.keyPixelPos[(int)InputKey::R					]= { 18,  8, 22, 12 };
	m_layout.keyPixelPos[(int)InputKey::S					]= { 11, 12, 15, 16 };
	m_layout.keyPixelPos[(int)InputKey::T					]= { 22,  8, 26, 12 };
	m_layout.keyPixelPos[(int)InputKey::U					]= { 30,  8, 34, 12 };
	m_layout.keyPixelPos[(int)InputKey::V					]= { 21, 16, 25, 20 };
	m_layout.keyPixelPos[(int)InputKey::W					]= { 10,  8, 14, 12 };
	m_layout.keyPixelPos[(int)InputKey::X					]= { 13, 16, 17, 20 };
	m_layout.keyPixelPos[(int)InputKey::Y					]= { 26,  8, 30, 12 };
	m_layout.keyPixelPos[(int)InputKey::Z					]= {  9, 16, 13, 20 };
	m_layout.keyPixelPos[(int)InputKey::Num_0				]= { 40,  4, 44,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_1				]= {  4,  4,  8,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_2				]= {  8,  4, 12,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_3				]= { 12,  4, 16,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_4				]= { 16,  4, 20,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_5				]= { 20,  4, 24,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_6				]= { 24,  4, 28,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_7				]= { 28,  4, 32,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_8				]= { 32,  4, 36,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_9				]= { 36,  4, 40,  8 };
	m_layout.keyPixelPos[(int)InputKey::Escape				]= {  0,  0,  4,  4 };
	m_layout.keyPixelPos[(int)InputKey::Tilde				]= {  0,  4,  4,  8 };
	m_layout.keyPixelPos[(int)InputKey::Plus				]= { 48,  4, 52,  8 };
	m_layout.keyPixelPos[(int)InputKey::Minus				]= { 44,  4, 48,  8 };
	m_layout.keyPixelPos[(int)InputKey::BackSpace			]= { 52,  4, 60,  8 };
	m_layout.keyPixelPos[(int)InputKey::Tab					]= {  0,  8,  6, 12 };
	m_layout.keyPixelPos[(int)InputKey::Bracket_Open		]= { 46,  8, 50, 12 }; 
	m_layout.keyPixelPos[(int)InputKey::Bracket_Close		]= { 50,  8, 54, 12 };
	m_layout.keyPixelPos[(int)InputKey::Backslash			]= { 54,  8, 60, 12 };
	m_layout.keyPixelPos[(int)InputKey::CapsLock			]= {  0, 12,  7, 16 };
	m_layout.keyPixelPos[(int)InputKey::Colon				]= { 43, 12, 47, 16 };
	m_layout.keyPixelPos[(int)InputKey::Apostrophe			]= { 47, 12, 51, 16 };
	m_layout.keyPixelPos[(int)InputKey::Enter				]= { 51, 12, 60, 16 };
	m_layout.keyPixelPos[(int)InputKey::Shift_Left			]= {  0, 16,  9, 20 };
	m_layout.keyPixelPos[(int)InputKey::Shift_Right			]= { 49, 16, 60, 20 };
	m_layout.keyPixelPos[(int)InputKey::Comma				]= { 37, 16, 41, 20 };
	m_layout.keyPixelPos[(int)InputKey::Dot					]= { 41, 16, 45, 20 };
	m_layout.keyPixelPos[(int)InputKey::ForwardSlash		]= { 45, 16, 49, 20 };
	m_layout.keyPixelPos[(int)InputKey::SpaceBar			]= { 15, 20, 40, 24 };
	m_layout.keyPixelPos[(int)InputKey::Fn					]= { 50, 20, 55, 24 };
	m_layout.keyPixelPos[(int)InputKey::Control_Left		]= {  0, 20,  5, 24 };
	m_layout.keyPixelPos[(int)InputKey::Control_Right		]= { 55, 20, 60, 24 };
	m_layout.keyPixelPos[(int)InputKey::Windows_Left		]= {  5, 20, 10, 24 };
	m_layout.keyPixelPos[(int)InputKey::Windows_Right		]= { 45, 20, 50, 24 };
	m_layout.keyPixelPos[(int)InputKey::Alt_Left			]= { 10, 20, 15, 24 };
	m_layout.keyPixelPos[(int)InputKey::Alt_Right			]= { 40, 20, 45, 24 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Up			]= { 64, 16, 68, 20 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Down			]= { 64, 20, 68, 24 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Left			]= { 60, 20, 64, 24 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Right			]= { 68, 20, 72, 24 };
	m_layout.keyPixelPos[(int)InputKey::Insert				]= { 60,  4, 64,  8 };
	m_layout.keyPixelPos[(int)InputKey::Delete				]= { 60,  8, 64, 12 };
	m_layout.keyPixelPos[(int)InputKey::Home				]= { 64,  4, 68,  8 };
	m_layout.keyPixelPos[(int)InputKey::End					]= { 64,  8, 68, 12 };
	m_layout.keyPixelPos[(int)InputKey::PageUp				]= { 68,  4, 72,  8 };
	m_layout.keyPixelPos[(int)InputKey::PageDown			]= { 68,  8, 72, 12 };
	m_layout.keyPixelPos[(int)InputKey::PrintScreen			]= { 60,  0, 64,  4 };
	m_layout.keyPixelPos[(int)InputKey::ScrollLock			]= { 64,  0, 68,  4 };
	m_layout.keyPixelPos[(int)InputKey::Pause				]= { 68,  0, 72,  4 };
	m_layout.keyPixelPos[(int)InputKey::F1					]= {  8,  0, 12,  4 };
	m_layout.keyPixelPos[(int)InputKey::F2					]= { 12,  0, 16,  4 };
	m_layout.keyPixelPos[(int)InputKey::F3					]= { 16,  0, 20,  4 };
	m_layout.keyPixelPos[(int)InputKey::F4					]= { 20,  0, 24,  4 };
	m_layout.keyPixelPos[(int)InputKey::F5					]= { 26,  0, 30,  4 };
	m_layout.keyPixelPos[(int)InputKey::F6					]= { 30,  0, 34,  4 };
	m_layout.keyPixelPos[(int)InputKey::F7					]= { 34,  0, 38,  4 };
	m_layout.keyPixelPos[(int)InputKey::F8					]= { 38,  0, 42,  4 };
	m_layout.keyPixelPos[(int)InputKey::F9					]= { 44,  0, 48,  4 };
	m_layout.keyPixelPos[(int)InputKey::F10					]= { 48,  0, 52,  4 };
	m_layout.keyPixelPos[(int)InputKey::F11					]= { 52,  0, 56,  4 };
	m_layout.keyPixelPos[(int)InputKey::F12					]= { 56,  0, 60,  4 };
}

void	RenderGraph::addLayoutKeyNumpad()
{
	m_layout.keyPixelPos[(int)InputKey::NumLock				]= { 72,  4, 76,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_0			]= { 72, 20, 80, 24 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_1			]= { 72, 16, 76, 20 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_2			]= { 76, 16, 80, 20 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_3			]= { 80, 16, 84, 20 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_4			]= { 72, 12, 76, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_5			]= { 76, 12, 80, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_6			]= { 80, 12, 84, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_7			]= { 72,  8, 76, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_8			]= { 76,  8, 80, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_9			]= { 80,  8, 84, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Plus			]= { 84,  8, 88, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Minus		]= { 84,  4, 88,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Multiply		]= { 80,  4, 84,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Divide		]= { 76,  4, 80,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Dot			]= { 80, 20, 84, 24 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Enter		]= { 84, 16, 88, 24 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra1		]= { 72,  0, 76,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra2		]= { 76,  0, 80,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra3		]= { 80,  0, 84,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra4		]= { 84,  0, 88,  4 };
}

void	RenderGraph::addLayoutKey_1800_ANSI()
{
	m_layout.keyPixelPos[(int)InputKey::A					]= {  7, 12, 11, 16 };
	m_layout.keyPixelPos[(int)InputKey::B					]= { 25, 16, 29, 20 };
	m_layout.keyPixelPos[(int)InputKey::C					]= { 17, 16, 21, 20 };
	m_layout.keyPixelPos[(int)InputKey::D					]= { 15, 12, 19, 16 };
	m_layout.keyPixelPos[(int)InputKey::E					]= { 14,  8, 18, 12 };
	m_layout.keyPixelPos[(int)InputKey::F					]= { 19, 12, 23, 16 };
	m_layout.keyPixelPos[(int)InputKey::G					]= { 23, 12, 27, 16 };
	m_layout.keyPixelPos[(int)InputKey::H					]= { 27, 12, 31, 16 };
	m_layout.keyPixelPos[(int)InputKey::I					]= { 34,  8, 38, 12 };
	m_layout.keyPixelPos[(int)InputKey::J					]= { 31, 12, 35, 16 };
	m_layout.keyPixelPos[(int)InputKey::K					]= { 35, 12, 39, 16 };
	m_layout.keyPixelPos[(int)InputKey::L					]= { 39, 12, 43, 16 };
	m_layout.keyPixelPos[(int)InputKey::M					]= { 33, 16, 37, 20 };
	m_layout.keyPixelPos[(int)InputKey::N					]= { 29, 16, 33, 20 };
	m_layout.keyPixelPos[(int)InputKey::O					]= { 38,  8, 42, 12 };
	m_layout.keyPixelPos[(int)InputKey::P					]= { 42,  8, 46, 12 };
	m_layout.keyPixelPos[(int)InputKey::Q					]= {  6,  8, 10, 12 };
	m_layout.keyPixelPos[(int)InputKey::R					]= { 18,  8, 22, 12 };
	m_layout.keyPixelPos[(int)InputKey::S					]= { 11, 12, 15, 16 };
	m_layout.keyPixelPos[(int)InputKey::T					]= { 22,  8, 26, 12 };
	m_layout.keyPixelPos[(int)InputKey::U					]= { 30,  8, 34, 12 };
	m_layout.keyPixelPos[(int)InputKey::V					]= { 21, 16, 25, 20 };
	m_layout.keyPixelPos[(int)InputKey::W					]= { 10,  8, 14, 12 };
	m_layout.keyPixelPos[(int)InputKey::X					]= { 13, 16, 17, 20 };
	m_layout.keyPixelPos[(int)InputKey::Y					]= { 26,  8, 30, 12 };
	m_layout.keyPixelPos[(int)InputKey::Z					]= {  9, 16, 13, 20 };
	m_layout.keyPixelPos[(int)InputKey::Num_0				]= { 40,  4, 44,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_1				]= {  4,  4,  8,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_2				]= {  8,  4, 12,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_3				]= { 12,  4, 16,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_4				]= { 16,  4, 20,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_5				]= { 20,  4, 24,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_6				]= { 24,  4, 28,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_7				]= { 28,  4, 32,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_8				]= { 32,  4, 36,  8 };
	m_layout.keyPixelPos[(int)InputKey::Num_9				]= { 36,  4, 40,  8 };
	m_layout.keyPixelPos[(int)InputKey::Escape				]= {  0,  0,  4,  4 };
	m_layout.keyPixelPos[(int)InputKey::Tilde				]= {  0,  4,  4,  8 };
	m_layout.keyPixelPos[(int)InputKey::Plus				]= { 48,  4, 52,  8 };
	m_layout.keyPixelPos[(int)InputKey::Minus				]= { 44,  4, 48,  8 };
	m_layout.keyPixelPos[(int)InputKey::BackSpace			]= { 52,  4, 60,  8 };
	m_layout.keyPixelPos[(int)InputKey::Tab					]= {  0,  8,  6, 12 };
	m_layout.keyPixelPos[(int)InputKey::Bracket_Open		]= { 46,  8, 50, 12 }; 
	m_layout.keyPixelPos[(int)InputKey::Bracket_Close		]= { 50,  8, 54, 12 };
	m_layout.keyPixelPos[(int)InputKey::Backslash			]= { 54,  8, 60, 12 };
	m_layout.keyPixelPos[(int)InputKey::CapsLock			]= {  0, 12,  7, 16 };
	m_layout.keyPixelPos[(int)InputKey::Colon				]= { 43, 12, 47, 16 };
	m_layout.keyPixelPos[(int)InputKey::Apostrophe			]= { 47, 12, 51, 16 };
	m_layout.keyPixelPos[(int)InputKey::Enter				]= { 51, 12, 60, 16 };
	m_layout.keyPixelPos[(int)InputKey::Shift_Left			]= {  0, 16,  9, 20 };
	m_layout.keyPixelPos[(int)InputKey::Shift_Right			]= { 49, 16, 56, 20 };
	m_layout.keyPixelPos[(int)InputKey::Comma				]= { 37, 16, 41, 20 };
	m_layout.keyPixelPos[(int)InputKey::Dot					]= { 41, 16, 45, 20 };
	m_layout.keyPixelPos[(int)InputKey::ForwardSlash		]= { 45, 16, 49, 20 };
	m_layout.keyPixelPos[(int)InputKey::SpaceBar			]= { 15, 20, 40, 24 };
	m_layout.keyPixelPos[(int)InputKey::Fn					]= { 46, 20, 52, 24 };
	m_layout.keyPixelPos[(int)InputKey::Control_Left		]= {  0, 20,  5, 24 };
	m_layout.keyPixelPos[(int)InputKey::Windows_Left		]= {  5, 20, 10, 24 };
	m_layout.keyPixelPos[(int)InputKey::Alt_Left			]= { 10, 20, 15, 24 };
	m_layout.keyPixelPos[(int)InputKey::Alt_Right			]= { 40, 20, 46, 24 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Up			]= { 56, 16, 60, 20 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Down			]= { 56, 20, 60, 24 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Left			]= { 52, 20, 56, 24 };
	m_layout.keyPixelPos[(int)InputKey::Arrow_Right			]= { 60, 20, 64, 24 };
	m_layout.keyPixelPos[(int)InputKey::Delete				]= { 72,  4, 76,  8 };
	m_layout.keyPixelPos[(int)InputKey::PrintScreen			]= { 56,  0, 60,  4 };
	m_layout.keyPixelPos[(int)InputKey::Pause				]= { 72,  0, 76,  4 };
	m_layout.keyPixelPos[(int)InputKey::F1					]= {  5,  0,  9,  4 };
	m_layout.keyPixelPos[(int)InputKey::F2					]= {  9,  0, 13,  4 };
	m_layout.keyPixelPos[(int)InputKey::F3					]= { 13,  0, 17,  4 };
	m_layout.keyPixelPos[(int)InputKey::F4					]= { 17,  0, 21,  4 };
	m_layout.keyPixelPos[(int)InputKey::F5					]= { 22,  0, 26,  4 };
	m_layout.keyPixelPos[(int)InputKey::F6					]= { 26,  0, 30,  4 };
	m_layout.keyPixelPos[(int)InputKey::F7					]= { 30,  0, 34,  4 };
	m_layout.keyPixelPos[(int)InputKey::F8					]= { 34,  0, 38,  4 };
	m_layout.keyPixelPos[(int)InputKey::F9					]= { 39,  0, 43,  4 };
	m_layout.keyPixelPos[(int)InputKey::F10					]= { 43,  0, 47,  4 };
	m_layout.keyPixelPos[(int)InputKey::F11					]= { 47,  0, 51,  4 };
	m_layout.keyPixelPos[(int)InputKey::F12					]= { 51,  0, 55,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumLock				]= { 60,  4, 64,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_0			]= { 64, 20, 68, 24 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_1			]= { 60, 16, 64, 20 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_2			]= { 64, 16, 68, 20 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_3			]= { 68, 16, 72, 20 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_4			]= { 60, 12, 64, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_5			]= { 64, 12, 68, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_6			]= { 68, 12, 72, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_7			]= { 60,  8, 64, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_8			]= { 64,  8, 68, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_9			]= { 68,  8, 72, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Plus			]= { 72, 12, 76, 16 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Minus		]= { 72,  8, 76, 12 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Multiply		]= { 68,  4, 72,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Divide		]= { 64,  4, 68,  8 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Dot			]= { 68, 20, 72, 24 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Enter		]= { 72, 16, 76, 24 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra1		]= { 60,  0, 64,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra2		]= { 64,  0, 68,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra3		]= { 68,  0, 72,  4 };
	m_layout.keyPixelPos[(int)InputKey::NumPad_Extra4		]= { 72,  0, 76,  4 };
}

void	RenderGraph::setGraph(RenderPass* pass)
{
	m_pass= pass;
	if (m_keyboard)
		pass->bindKeyboard(m_keyboard);
}

float	RenderGraph::update(float deltaSeconds)
{
	return m_pass->update(m_keyboard, deltaSeconds);
}

void	RenderGraph::render()
{
	assert(m_pass != nullptr);
	RenderTarget* renderTarget= m_renderTargetPool.allocRT();
	m_pass->render(&m_layout, &m_renderTargetPool, renderTarget);
	setKeyboardColor(renderTarget);
	setMouseColor(renderTarget);
	m_renderTargetPool.freeRT(renderTarget);
}

void	RenderGraph::setKeyboardColor(RenderTarget* renderTarget)
{
	if (m_keyboard == nullptr)
		return;

	for(int i=0; i<(int)InputKey::Num; ++i)
	{
		int4	pxRange		= m_layout.keyPixelPos[i];
		unsigned char R, G, B;
		if (renderTarget->getAverageColorInRange(pxRange, &R, &G, &B))
			m_keyboard->setKeyColor((InputKey)i, R, G, B);
	}
	m_keyboard->commitKeyColor(renderTarget);
}

void	RenderGraph::setMouseColor(RenderTarget* renderTarget)
{
	if (m_mouse == nullptr)
		return;
	int2	rtSz		= renderTarget->size;
	int4	pxRange		= // use the same color as the right most key of the keyboard
	{
		rtSz.x - MOUSE_PX_WIDTH,
		0,
		rtSz.x,
		rtSz.y,
	};
	float R, G, B;
	if (renderTarget->getAverageColorInRange(pxRange, &R, &G, &B))
		m_mouse->setColor(R, G, B);
}
