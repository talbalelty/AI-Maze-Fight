#include "Crate.h"

Crate::Crate() {

}
Crate::~Crate() {

}

Crate::Crate(Cell* newCell, bool _type) {
	cell = newCell;
	type = _type;
}