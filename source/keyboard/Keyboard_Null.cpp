
// by simon yeung, 05/10/2021
// all rights reserved

#include "Keyboard_Null.h"

Keyboard_Null::Keyboard_Null() : Keyboard()
{
	beginAddKey();
	addKeyTKL_ANSI();
	endAddKey();
}

Keyboard_Null::~Keyboard_Null()
{
}

void Keyboard_Null::commitKeyColor()
{
}

void	Keyboard_Null::connect()
{
}

void	Keyboard_Null::disconnect()
{
}
