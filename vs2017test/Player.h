#pragma once
#include "Cell.h"
#include "Room.h"
#include <math.h>

#define WALK 0
#define FIGHT 1
#define SURVIVE 2
#define DEAD 3
#define HOLD 4
#define DISTANCE_TO_TARGET 10

class Player
{
private:
	Cell* cell;
	int health;
	int grenadeAmmo;
	int bulletAmmo;
	bool isFighter;
public:
	Player();
	Player(Cell* newCell, bool _isFighter);
	~Player();
	void setCell(Cell* other) { cell = other; };
	Cell* getCell() { return cell; };
	bool getIsFighter() { return isFighter; }
	int getState(Player* opponent, Room* rooms[NUM_ROOMS]);
	int getRoomNumber(Room* rooms[NUM_ROOMS]);
	
};
