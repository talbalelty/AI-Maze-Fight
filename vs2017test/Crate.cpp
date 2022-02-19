#include "Crate.h"

Crate::Crate() {

}
Crate::~Crate() {

}

Crate::Crate(Cell* newCell, int _type) {
	cell = newCell;
	type = _type;
}