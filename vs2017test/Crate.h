#pragma once
#include "Definitions.h"
#include "Cell.h"

class Crate {
private:
	Cell* cell;
	bool type; // 0 = health, 1 = ammo
public:
	Crate();
	Crate(Cell *newCell, bool type);
	~Crate();
};