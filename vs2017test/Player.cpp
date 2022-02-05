#include "Player.h"

Player::Player() {
	health = 10;
	bulletAmmo = 10;
	grenadeAmmo = 2;
}

Player::Player(Cell* newCell, bool _isFighter) {
	health = 10;
	bulletAmmo = 10;
	grenadeAmmo = 2;
	cell = newCell;
	isFighter = _isFighter;
}

Player::~Player() {

}

int Player::getState(Player* opponent) {
	if (health == 0)
	{
		return DEAD;
	}
	if (health <= 2 || (bulletAmmo == 0  && grenadeAmmo == 0))
	{
		return SURVIVE;
	}
	int col = cell->getCol();
	int row = cell->getRow();
	int oppCol = opponent->getCell()->getCol();
	int oppRow = opponent->getCell()->getRow();
	double distance = sqrt(pow(oppCol - col, 2) + pow(oppRow - row, 2));
	if (distance <= DISTANCE_TO_TARGET)
	{
		return FIGHT;
	}
	return WALK;
}
