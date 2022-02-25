#pragma once
#include "Cell.h"
#include "Room.h"
#include <math.h>
#include <vector>
#include "Bullet.h"
#include "Definitions.h"

#define WALK 0
#define FIGHT 1
#define SURVIVE 2
#define DEAD 3
#define HOLD 4
#define DISTANCE_TO_TARGET 10
#define MINIMUM_HEALTH 2
#define MAX_HEALTH 10

using namespace std;

class Player
{
private:
	Cell* cell;
	int health;
	int grenadeAmmo;
	int bulletAmmo;
	int crateType;
	int team;
	bool isFighter;
	vector<Bullet*> bullets;
public:
	Player();
	Player(Cell* newCell, bool _isFighter, int _color);
	~Player();
	void setCell(Cell* other) { cell = other; }
	Cell* getCell() { return cell; }
	bool getIsFighter() { return isFighter; }
	int getState(Player* opponent, Room* rooms[NUM_ROOMS]);
	int getRoomNumber(Room* rooms[NUM_ROOMS]);
	vector<Bullet*> getBullets() { return bullets; }
	void setBullets(vector<Bullet*> _bullets) { bullets = _bullets; }
	void takeDamage(int damage) { health -= abs(damage); }
	int getBulletAmmo() { return bulletAmmo; }
	void fireBullet() { bulletAmmo--; }
	int getGrenadeAmmo() { return grenadeAmmo; }
	void throwGrenade() { grenadeAmmo--; }
	void restockAmmo();
	void heal();
	int getTeam() { return team; }
	int getSupportType();
	int getHealth() { return health; }
	int getCrateType() { return crateType; }
	void setCrateType(int _crateType) { crateType = _crateType; }
};
