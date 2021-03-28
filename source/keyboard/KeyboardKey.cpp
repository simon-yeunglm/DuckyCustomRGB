
// by simon yeung, 21/10/2020
// all rights reserved

#include "KeyboardKey.h"
#include "win.h"
#include <assert.h>

static const InputKey	s_scanCodeToKey[]=
{
	InputKey::Num			, InputKey::Escape			, InputKey::Num_1		, InputKey::Num_2		, InputKey::Num_3		, InputKey::Num_4			, InputKey::Num_5		, InputKey::Num_6			, InputKey::Num_7		, InputKey::Num_8		, 	// 0   - 9
	InputKey::Num_9			, InputKey::Num_0			, InputKey::Minus		, InputKey::Plus		, InputKey::BackSpace	, InputKey::Tab				, InputKey::Q			, InputKey::W				, InputKey::E			, InputKey::R			, 	// 10  - 19
	InputKey::T				, InputKey::Y				, InputKey::U			, InputKey::I			, InputKey::O			, InputKey::P				, InputKey::Bracket_Open, InputKey::Bracket_Close	, InputKey::Enter		, InputKey::Control_Left	, 	// 20  - 29
	InputKey::A				, InputKey::S				, InputKey::D			, InputKey::F			, InputKey::G			, InputKey::H				, InputKey::J			, InputKey::K				, InputKey::L			, InputKey::Colon		, 	// 30  - 39
	InputKey::Apostrophe	, InputKey::Tilde			, InputKey::Shift_Left	, InputKey::Backslash	, InputKey::Z			, InputKey::X				, InputKey::C			, InputKey::V				, InputKey::B			, InputKey::N			, 	// 40  - 49
	InputKey::M				, InputKey::Comma			, InputKey::Dot			, InputKey::ForwardSlash, InputKey::Shift_Right	, InputKey::NumPad_Multiply	, InputKey::Alt_Left	, InputKey::SpaceBar		, InputKey::CapsLock	, InputKey::F1			, 	// 50  - 59
	InputKey::F2			, InputKey::F3				, InputKey::F4			, InputKey::F5			, InputKey::F6			, InputKey::F7				, InputKey::F8			, InputKey::F9				, InputKey::F10			, InputKey::Pause		, 	// 60  - 69	
	InputKey::ScrollLock	, InputKey::NumPad_7		, InputKey::NumPad_8	, InputKey::NumPad_9	, InputKey::NumPad_Minus, InputKey::NumPad_4		, InputKey::NumPad_5	, InputKey::NumPad_6		, InputKey::NumPad_Plus	, InputKey::NumPad_1		, 	// 70  - 79
	InputKey::NumPad_2		, InputKey::NumPad_3		, InputKey::NumPad_0	, InputKey::NumPad_Dot	, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::F11				, InputKey::F12			, InputKey::Num			, 	// 80  - 89
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 90  - 99
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 100 - 109
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 110 - 119
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 130 - 139
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 120 - 129
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 140 - 149
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::NumPad_Enter, InputKey::Control_Right	, InputKey::Num			, InputKey::Num			, 	// 150 - 159
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 160 - 169
	InputKey::PrintScreen	, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 170 - 179
	InputKey::Num			, InputKey::NumPad_Divide	, InputKey::Num			, InputKey::Num			, InputKey::Alt_Right	, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 180 - 189
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Home		, 	// 190 - 199
	InputKey::Arrow_Up		, InputKey::PageUp			, InputKey::Num			, InputKey::Arrow_Left	, InputKey::Num			, InputKey::Arrow_Right		, InputKey::Num			, InputKey::End				, InputKey::Arrow_Down	, InputKey::PageDown	, 	// 200 - 209
	InputKey::Insert		, InputKey::Delete			, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Windows_Left, 	// 210 - 219
	InputKey::Windows_Right	, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 220 - 229
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 230 - 239
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 	// 240 - 249
	InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num				, InputKey::Num			, InputKey::Num			, 
	InputKey::Num			, InputKey::Num				, InputKey::Num
};

static_assert(((int)InputKey::Num) == 115, "Please update s_keyToStr array");
static const char* s_keyToStr[1+(int)InputKey::Num]
{
	"A"					, 
	"B"					, 
	"C"					, 
	"D"					, 
	"E"					, 
	"F"					, 
	"G"					, 
	"H"					, 
	"I"					, 
	"J"					, 
	"K"					, 
	"L"					, 
	"M"					, 
	"N"					, 
	"O"					, 
	"P"					, 
	"Q"					, 
	"R"					, 
	"S"					, 
	"T"					, 
	"U"					, 
	"V"					, 
	"W"					, 
	"X"					, 
	"Y"					, 
	"Z"					, 

	"0"					, 
	"1"					, 
	"2"					, 
	"3"					, 
	"4"					, 
	"5"					, 
	"6"					, 
	"7"					, 
	"8"					, 
	"9"					, 

	"Escape"			,

	"Tilde"				,
	"Plus"				,
	"Minus"				,
	"BackSpace"			,

	"Tab"				,
	"["					,
	"]"					,
	"\\"				,

	"CapsLock"			,
	":"					,
	"'"					,
	"Enter"				,

	"Shift_Left"		,
	"Shift_Right"		,
	","					,
	"."					,
	"/"				,

	"SpaceBar"			,
	"Fn"				,
	"Control_Left"		,
	"Control_Right"		,
	"Windows_Left"		,
	"Windows_Right"		,
	"Alt_Left"			,
	"Alt_Right"			,

	"Arrow_Up"			,
	"Arrow_Down"		,
	"Arrow_Left"		,
	"Arrow_Right"		,

	"Insert"			,
	"Delete"			,
	"Home"				,
	"End"				,
	"PageUp"			,
	"PageDown"			,

	"PrintScreen"		,
	"ScrollLock"		,
	"Pause"				,

	"F1"				,
	"F2"				,
	"F3"				,
	"F4"				,
	"F5"				,
	"F6"				,
	"F7"				,
	"F8"				,
	"F9"				,
	"F10"				,
	"F11"				,
	"F12"				,

	"NumLock"			,

	"NumPad 0"			,
	"NumPad 1"			,
	"NumPad 2"			,
	"NumPad 3"			,
	"NumPad 4"			,
	"NumPad 5"			,
	"NumPad 6"			,
	"NumPad 7"			,
	"NumPad 8"			,
	"NumPad 9"			,
	"NumPad +"			,
	"NumPad -"			,
	"NumPad *"			,
	"NumPad /"			,
	"NumPad ."			,
	"NumPad Enter"		,

	"NumPad Extra1"		,
	"NumPad Extra2"		,
	"NumPad Extra3"		,
	"NumPad Extra4"		,
		
	"Mouse Left"		,
	"Mouse Right"		,
	"Mouse Middle"		,
	"Mouse Forward"		,
	"Mouse Back"		,
	"Mouse ScrollUp"	,
	"Mouse ScrollDown"	,

	"Unknown"			,
};

static unsigned char s_keyToVK[1+(int)InputKey::Num]
{
	0x41	, 
	0x42	, 
	0x43	, 
	0x44	, 
	0x45	, 
	0x46	, 
	0x47	, 
	0x48	, 
	0x49	, 
	0x4a	, 
	0x4b	, 
	0x4c	, 
	0x4d	, 
	0x4e	, 
	0x4f	, 
	0x50	, 
	0x51	, 
	0x52	, 
	0x53	, 
	0x54	, 
	0x55	, 
	0x56	, 
	0x57	, 
	0x58	, 
	0x59	, 
	0x5a	, 

	0x30			, 
	0x31			, 
	0x32			, 
	0x33			, 
	0x34			, 
	0x35			, 
	0x36			, 
	0x37			, 
	0x38			, 
	0x39			, 

	VK_ESCAPE		,

	VK_OEM_3		,
	VK_OEM_PLUS		,
	VK_OEM_MINUS	,
	VK_BACK			,

	VK_TAB			,
	VK_OEM_4		,
	VK_OEM_6		,
	VK_OEM_5		,

	VK_CAPITAL		,
	VK_OEM_1		,
	VK_OEM_7		,
	VK_RETURN		,

	VK_LSHIFT		,
	VK_RSHIFT		,
	VK_OEM_COMMA	,
	VK_OEM_PERIOD	,
	VK_OEM_2		,

	VK_SPACE		,
	0				,	// no this key
	VK_LCONTROL		,
	VK_RCONTROL		,
	VK_LWIN			,
	VK_RWIN			,
	VK_LMENU		,
	VK_RMENU		,

	VK_UP			,
	VK_DOWN			,
	VK_LEFT			,
	VK_RIGHT		,

	VK_INSERT		,
	VK_DELETE		,
	VK_HOME			,
	VK_END			,
	VK_PRIOR		,
	VK_NEXT			,

	VK_PRINT		,
	VK_SCROLL		,
	VK_PAUSE		,

	VK_F1			,
	VK_F2			,
	VK_F3			,
	VK_F4			,
	VK_F5			,
	VK_F6			,
	VK_F7			,
	VK_F8			,
	VK_F9			,
	VK_F10			,
	VK_F11			,
	VK_F12			,

	VK_NUMLOCK		,

	VK_NUMPAD0		,
	VK_NUMPAD1		,
	VK_NUMPAD2		,
	VK_NUMPAD3		,
	VK_NUMPAD4		,
	VK_NUMPAD5		,
	VK_NUMPAD6		,
	VK_NUMPAD7		,
	VK_NUMPAD8		,
	VK_NUMPAD9		,

	// TODO: just use normal key instead of numpad key for now...
	VK_ADD			,
	VK_SUBTRACT		,
	VK_MULTIPLY		,
	VK_DIVIDE		,
	VK_DECIMAL		,
	VK_RETURN		,

	0	,	// no this key
	0	,	// no this key
	0	,	// no this key
	0	,	// no this key

	// mouse
	VK_LBUTTON	,
	VK_RBUTTON	,
	VK_MBUTTON	,
	VK_XBUTTON1	,
	VK_XBUTTON2	,
	0			,
	0			,
};

const char* KeyboardKeyToString(InputKey key)
{
	assert((int)key <= (int)InputKey::Num);
	return s_keyToStr[(int)key];
}

InputKey	KeyboardKeyFromScanCode(int scanCode, bool hasPrefixE0)
{
	scanCode+= hasPrefixE0 ? 128 : 0;
	assert(scanCode < (sizeof(s_scanCodeToKey)/sizeof(InputKey)));
	return s_scanCodeToKey[scanCode];
}

unsigned char	KeyboardKeyToVirtualKeyCode(InputKey key)
{
	assert((int)key <= (int)InputKey::Num);
	return s_keyToVK[(int)key];
}
