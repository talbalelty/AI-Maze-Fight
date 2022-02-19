#pragma once
#include "Definitions.h"
#include "Cell.h"

class Crate {
private:
	Cell* cell;
	int type; // 0 = health, 1 = ammo
public:
	Crate();
	Crate(Cell *newCell, int type);
	~Crate();
	Cell* getCell() { return cell; }
	int getType() { return type; }
};