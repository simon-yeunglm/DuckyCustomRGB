
// by simon yeung, 21/10/2020
// all rights reserved

#include "KeyboardKey.h"
#include "win.h"
#include <assert.h>

static const KeyboardKey	s_scanCodeToKey[]=
{
	KeyboardKey::Num			, KeyboardKey::Escape			, KeyboardKey::Num_1		, KeyboardKey::Num_2		, KeyboardKey::Num_3		, KeyboardKey::Num_4			, KeyboardKey::Num_5		, KeyboardKey::Num_6			, KeyboardKey::Num_7		, KeyboardKey::Num_8		, 	// 0   - 9
	KeyboardKey::Num_9			, KeyboardKey::Num_0			, KeyboardKey::Minus		, KeyboardKey::Plus			, KeyboardKey::BackSpace	, KeyboardKey::Tab				, KeyboardKey::Q			, KeyboardKey::W				, KeyboardKey::E			, KeyboardKey::R			, 	// 10  - 19
	KeyboardKey::T				, KeyboardKey::Y				, KeyboardKey::U			, KeyboardKey::I			, KeyboardKey::O			, KeyboardKey::P				, KeyboardKey::Bracket_Open	, KeyboardKey::Bracket_Close	, KeyboardKey::Enter		, KeyboardKey::Control_Left	, 	// 20  - 29
	KeyboardKey::A				, KeyboardKey::S				, KeyboardKey::D			, KeyboardKey::F			, KeyboardKey::G			, KeyboardKey::H				, KeyboardKey::J			, KeyboardKey::K				, KeyboardKey::L			, KeyboardKey::Colon		, 	// 30  - 39
	KeyboardKey::Apostrophe		, KeyboardKey::Tilde			, KeyboardKey::Shift_Left	, KeyboardKey::Backslash	, KeyboardKey::Z			, KeyboardKey::X				, KeyboardKey::C			, KeyboardKey::V				, KeyboardKey::B			, KeyboardKey::N			, 	// 40  - 49
	KeyboardKey::M				, KeyboardKey::Comma			, KeyboardKey::Dot			, KeyboardKey::ForwardSlash	, KeyboardKey::Shift_Right	, KeyboardKey::NumPad_Multiply	, KeyboardKey::Alt_Left		, KeyboardKey::SpaceBar			, KeyboardKey::CapsLock		, KeyboardKey::F1			, 	// 50  - 59
	KeyboardKey::F2				, KeyboardKey::F3				, KeyboardKey::F4			, KeyboardKey::F5			, KeyboardKey::F6			, KeyboardKey::F7				, KeyboardKey::F8			, KeyboardKey::F9				, KeyboardKey::F10			, KeyboardKey::Pause		, 	// 60  - 69	
	KeyboardKey::ScrollLock		, KeyboardKey::NumPad_7			, KeyboardKey::NumPad_8		, KeyboardKey::NumPad_9		, KeyboardKey::NumPad_Minus	, KeyboardKey::NumPad_4			, KeyboardKey::NumPad_5		, KeyboardKey::NumPad_6			, KeyboardKey::NumPad_Plus	, KeyboardKey::NumPad_1		, 	// 70  - 79
	KeyboardKey::NumPad_2		, KeyboardKey::NumPad_3			, KeyboardKey::NumPad_0		, KeyboardKey::NumPad_Dot	, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::F11				, KeyboardKey::F12			, KeyboardKey::Num			, 	// 80  - 89
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 90  - 99
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 100 - 109
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 110 - 119
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 130 - 139
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 120 - 129
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 140 - 149
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::NumPad_Enter	, KeyboardKey::Control_Right	, KeyboardKey::Num			, KeyboardKey::Num			, 	// 150 - 159
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 160 - 169
	KeyboardKey::PrintScreen	, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 170 - 179
	KeyboardKey::Num			, KeyboardKey::NumPad_Divide	, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Alt_Right	, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 180 - 189
	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Home			, 	// 190 - 199
	KeyboardKey::Arrow_Up		, KeyboardKey::PageUp			, KeyboardKey::Num			, KeyboardKey::Arrow_Left	, KeyboardKey::Num			, KeyboardKey::Arrow_Right		, KeyboardKey::Num			, KeyboardKey::End				, KeyboardKey::Arrow_Down	, KeyboardKey::PageDown		, 	// 200 - 209
	KeyboardKey::Insert			, KeyboardKey::Delete			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Windows_Left	, 	// 210 - 219
	KeyboardKey::Windows_Right	//, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 220 - 229
//	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 230 - 239
//	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, 	// 240 - 249
//	KeyboardKey::Num			, KeyboardKey::Num				, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num			, KeyboardKey::Num				, //KeyboardKey::Num	, KeyboardKey::Num	, KeyboardKey::Num	, KeyboardKey::Num	, 
};

static_assert(((int)KeyboardKey::Num) == 108, "Please update s_keyToStr array");
static const char* s_keyToStr[1+(int)KeyboardKey::Num]
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

	"Unknown"			,
};

static unsigned char s_keyToVK[1+(int)KeyboardKey::Num]
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
};

const char* KeyboardKeyToString(KeyboardKey key)
{
	assert((int)key <= (int)KeyboardKey::Num);
	return s_keyToStr[(int)key];
}

KeyboardKey	KeyboardKeyFromScanCode(int scanCode, bool hasPrefixE0)
{
	scanCode+= hasPrefixE0 ? 128 : 0;
	assert(scanCode < (sizeof(s_scanCodeToKey)/sizeof(KeyboardKey)));
	return s_scanCodeToKey[scanCode];
}

unsigned char	KeyboardKeyToVirtualKeyCode(KeyboardKey key)
{
	assert((int)key <= (int)KeyboardKey::Num);
	return s_keyToVK[(int)key];
}
