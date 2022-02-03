#include "Cell.h"
#include <math.h>

Cell::Cell()
{

}

Cell::Cell(const Cell& other) {
	row = other.row;
	col = other.col;
	target_row = other.target_row;
	target_col = other.target_col;
	parent = other.parent;
	g = other.g;
	h = other.h;
	f = h + g;
}

Cell::Cell(int r, int c,int tr, int tc,double g,Cell * p)
{
	row = r;
	col = c;
	target_row = tr;
	target_col = tc;
	parent = p;
	this->g = g;
	h = sqrt(pow(r - tr, 2) + pow(c - tc, 2));
	f = h + g;
}

Cell::Cell(int r, int c)
{
	row = r;
	col = c;
}
Cell::~Cell()
{
}
