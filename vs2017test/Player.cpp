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

int Player::getState(Player* opponent, Room* rooms[NUM_ROOMS]) {
	if (health <= 0)
	{
		return DEAD;
	}
	if (health <= 2 || (bulletAmmo == 0  && grenadeAmmo == 0))
	{
		return SURVIVE;
	}
	//int col = cell->getCol();
	//int row = cell->getRow();
	//int oppCol = opponent->getCell()->getCol();
	//int oppRow = opponent->getCell()->getRow();
	//double distance = sqrt(pow(oppCol - col, 2) + pow(oppRow - row, 2));
	//bool isInFightDistance = distance <= DISTANCE_TO_TARGET;
	int playerRoom = getRoomNumber(rooms);
	if (opponent)
	{
		int opponentRoom = opponent->getRoomNumber(rooms);
		if (playerRoom == opponentRoom)
		{
			return FIGHT;
		} 
		// Player will HOLD
		if (playerRoom != -1 && opponentRoom == -1)
		{
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
