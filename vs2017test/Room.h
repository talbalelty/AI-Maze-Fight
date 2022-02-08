#pragma once
class Room
{
private:
	int w, h; //absolute numbers
	int center_row, center_col; //indexes
public:
	Room();
	~Room();
	Room(int cr, int cc, int w, int h);
	int getW() { return w; }
	int getH() { return h; }
	int getCenterRow() { return center_row; }
	int getCenterCol() { return center_col; }
};

