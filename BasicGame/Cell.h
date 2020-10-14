#pragma once
#include "CellType.h"

struct Cell
{
	CellType type;
	// when the last change was on this cell
	// if the timestamp matches check type
	// otherwise it is assumed free or block
	size_t timestamp;
};

