#include "Player.h"

Player::Player() {
	health = 10;
	bulletAmmo = 10;
	grenadeAmmo = 2;
}

Player::Player(Cell* newCell, bool _isFighter, int _color) {
	health = 10;
	bulletAmmo = 10;
	grenadeAmmo = 2;
	cell = newCell;
	isFighter = _isFighter;
	color = _color;
}

Player::~Player() {

}

int Player::getState(Player* opponent, Room* rooms[NUM_ROOMS]) {
	int col = cell->getCol();
	int row = cell->getRow();
	if (health <= 0)
	{
		return DEAD;
	}
	if (health <= 2 || (bulletAmmo == 0  && grenadeAmmo == 0))
	{
		return SURVIVE;
	}
	if (opponent)
	{
		int oppCol = opponent->getCell()->getCol();
		int oppRow = opponent->getCell()->getRow();
		double distance = sqrt(pow(oppCol - col, 2) + pow(oppRow - row, 2));
		int playerRoom = getRoomNumber(rooms);
		int opponentRoom = opponent->getRoomNumber(rooms);
		bool isInSameRoom = playerRoom == opponentRoom;
		bool isInFightDistance = distance <= DISTANCE_TO_TARGET && isInSameRoom;
		if (isInFightDistance)
		{
			return FIGHT;
		} 
		// Player will HOLD
		bool isInCorridor = playerRoom != -1 && opponentRoom == -1;
		bool isInHoldDistance = distance <= DISTANCE_TO_TARGET && isInCorridor;
		if (isInHoldDistance)
		{
			// TODO - Find a better solution to hold condition!
			return HOLD;
		}
	}
	// else WALK
	return WALK;
}

int Player::getRoomNumber(Room* rooms[NUM_ROOMS]) {
	Room room;
	int centerRow, centerCol, h, w;
	bool inRow, inCol;
	for (size_t i = 0; i < NUM_ROOMS; i++)
	{
		room = *rooms[i];
		centerRow = room.getCenterRow();
		centerCol = room.getCenterCol();
		h = room.getH() / 2;
		w = room.getW() / 2;
		inRow = (centerRow - h <= cell->getRow()) && (cell->getRow() <= centerRow + h);
		inCol = (centerCol - w <= cell->getCol()) && (cell->getCol() <= centerCol + w);
		if (inRow && inCol)
		{
			return i;
		}
	}
	return -1;
}

void Player::restockAmmo() {
	bulletAmmo += 5;
	grenadeAmmo += 1;
}
