#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

enum class KeyboardKey
{
	A					, 
	B					, 
	C					, 
	D					, 
	E					, 
	F					, 
	G					, 
	H					, 
	I					, 
	J					, 
	K					, 
	L					, 
	M					, 
	N					, 
	O					, 
	P					, 
	Q					, 
	R					, 
	S					, 
	T					, 
	U					, 
	V					, 
	W					, 
	X					, 
	Y					, 
	Z					, 

	Num_0				, 
	Num_1				, 
	Num_2				, 
	Num_3				, 
	Num_4				, 
	Num_5				, 
	Num_6				, 
	Num_7				, 
	Num_8				, 
	Num_9				, 
	
	Escape				,

	Tilde				,
	Plus				,
	Minus				,
	BackSpace			,
	
	Tab					,
	Bracket_Open		,
	Bracket_Close		,
	Backslash			,
	
	CapsLock			,
	Colon				,
	Apostrophe			,
	Enter				,
	
	Shift_Left			,
	Shift_Right			,
	Comma				,
	Dot					,
	ForwardSlash		,
	
	SpaceBar			,
	Fn					,
	Control_Left		,
	Control_Right		,
	Windows_Left		,
	Windows_Right		,
	Alt_Left			,
	Alt_Right			,
	
	Arrow_Up			,
	Arrow_Down			,
	Arrow_Left			,
	Arrow_Right			,

	Insert				,
	Delete				,
	Home				,
	End					,
	PageUp				,
	PageDown			,

	PrintScreen			,
	ScrollLock			,
	Pause				,

	F1					,
	F2					,
	F3					,
	F4					,
	F5					,
	F6					,
	F7					,
	F8					,
	F9					,
	F10					,
	F11					,
	F12					,
	
	NumLock				,

	NumPad_0			,
	NumPad_1			,
	NumPad_2			,
	NumPad_3			,
	NumPad_4			,
	NumPad_5			,
	NumPad_6			,
	NumPad_7			,
	NumPad_8			,
	NumPad_9			,
	NumPad_Plus			,
	NumPad_Minus		,
	NumPad_Multiply		,
	NumPad_Divide		,
	NumPad_Dot			,
	NumPad_Enter		,

	NumPad_Extra1		,
	NumPad_Extra2		,
	NumPad_Extra3		,
	NumPad_Extra4		,

	Num					,	// Total number of key caps
};

const char*		KeyboardKeyToString(KeyboardKey key);
KeyboardKey		KeyboardKeyFromScanCode(int scanCode, bool hasPrefixE0);
unsigned char	KeyboardKeyToVirtualKeyCode(KeyboardKey key);
