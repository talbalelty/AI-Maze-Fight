#pragma once
#include "Definitions.h"

class Cell
{
private:
	int row, col;
	Cell* parent;
	int target_row, target_col;
	double h, g, f;

public:
	Cell();
	Cell(const Cell& other);
	Cell(int r, int c, Cell* p);
	Cell(int r, int c, int tr, int tc, double g, Cell * p);
	Cell(int r, int c);
	~Cell();
	int getRow() { return row; }
	int getCol() { return col; }
	int getTargetRow() { return target_row; }
	int getTargetCol() { return target_col; }
	void setTargetRow(int row) { target_row = row; }
	void setTargetCol(int col) { target_col = col; }
	double getF() { return f; }
	double getG() { return g; }
	bool operator == (const Cell &other) { return other.col == col && other.row == row; }
	void setG(double newG) { g = newG; }
	void setF(double newF) { f = newF; }
	void setF() { f = h + g; }
	Cell* getParent() { return parent; }
	int ManhattanDistance(int tRow, int tCol);
	void computeH(Cell* target);
};

