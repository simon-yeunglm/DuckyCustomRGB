#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

#include "math/vector.h"
#include "keyboard/KeyboardKey.h"

struct RenderLayout
{
	// AABB represent key pixel boundaries
	// xy= min coordinates, inclusive, zw= max coordinates, exclusive
	int4	keyPixelPos[(int)KeyboardKey::Num];
};
