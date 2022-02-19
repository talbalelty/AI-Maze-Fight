#include "Arena.h"

Arena::Arena(int(*_maze)[MSZ][MSZ], Room* (*_rooms)[NUM_ROOMS], double(*_security_map)[MSZ][MSZ]) {
	maze = _maze;
	rooms = _rooms;
	security_map = _security_map;
	initCrates();
	initTeams();
}

void Arena::initTeams() {
	Room room1 = *(*rooms)[(rand() % (NUM_ROOMS / 2)) + (NUM_ROOMS / 2)];
	Room room2 = *(*rooms)[rand() % (NUM_ROOMS / 2)];
	int room1row = room1.getCenterRow(), room1col = room1.getCenterCol();
	int room2row = room2.getCenterRow(), room2col = room2.getCenterCol();

	//First player is support
	for (size_t i = 0; i < TEAM_SIZE; i++)
	{
		team1.push_back(new Player(new Cell(room1row + i, room1col + i), i != 0, TEAM1));
		(*maze)[room1row + i][room1col + i] = TEAM1;
		team2.push_back(new Player(new Cell(room2row + i, room2col + i), i != 0, TEAM2));
		(*maze)[room2row + i][room2col + i] = TEAM2;
	}
}

void Arena::initCrates() {
	Room room;
	int row, col;

	for (size_t i = 0; i < NUM_ROOMS; i++)
	{
		room = *(*rooms)[i];
		row = (room.getCenterRow() - room.getH() / 2) + (rand() % room.getH());
		col = (room.getCenterCol() - room.getW() / 2) + (rand() % room.getW());
		crates.push_back(new Crate(new Cell(row, col), AMMO));
		(*maze)[row][col] = AMMO;
		row = (room.getCenterRow() - room.getH() / 2) + (rand() % room.getH());
		col = (room.getCenterCol() - room.getW() / 2) + (rand() % room.getW());
		crates.push_back(new Crate(new Cell(row, col), HEALTH));
		(*maze)[row][col] = HEALTH;
	}
}

/// <summary>
/// check to see if runs
/// </summary>
/// <param name="player"></param>
/// <param name="team"></param>
/// <returns></returns>
Player* Arena::findOpponent(Player* player, vector<Player*> team) {
	Cell* playerCell = player->getCell();
	Player* opp = nullptr;
	double minManhattan = LONG_MAX;
	double newManhattan = LONG_MAX;
	for (size_t i = 1; i < TEAM_SIZE; i++)
	{
		if (team[i]->getState(player, *rooms) != DEAD)
		{
			if (opp == nullptr)
			{
				opp = team[i];
				minManhattan = playerCell->ManhattanDistance(opp->getCell()->getRow(), opp->getCell()->getCol());
			}
			newManhattan = playerCell->ManhattanDistance(team[i]->getCell()->getRow(), team[i]->getCell()->getCol());
			if (newManhattan < minManhattan)
			{
				minManhattan = newManhattan;
				opp = team[i];
			}
		}
	}
	return opp;
}

void Arena::show() {
	Player* opponent = nullptr;
	for (size_t i = 0; i < TEAM_SIZE; i++)
	{
		opponent = findOpponent(team1[i], team2);
		if (opponent)
		{
			moveBullets(team1[i], opponent);
		}
	}
	for (size_t i = 0; i < TEAM_SIZE; i++)
	{
		opponent = findOpponent(team2[i], team1);
		if (opponent)
		{
			moveBullets(team2[i], opponent);
		}
	}
}

void Arena::iteration() {
	// for TEAM_SIZE - each iteration calls playerStance for each player. 6 calls total.
	Player* opponent = nullptr;
	for (size_t i = 0; i < TEAM_SIZE; i++)
	{
		opponent = findOpponent(team1[i], team2);
		if (opponent) {
			playerAction(team1[i], opponent);
		}
	}
	for (size_t i = 0; i < TEAM_SIZE; i++)
	{
		opponent = findOpponent(team2[i], team1);
		if (opponent)
		{
			playerAction(team2[i], opponent);
		}
	}
}

void Arena::playerAction(Player* player, Player* opponent) {
	int col = player->getCell()->getCol();
	int row = player->getCell()->getRow();
	if (player->getIsFighter())
	{
		int state = player->getState(opponent, *rooms);
		switch (state)
		{
		case WALK:
			walk(player, opponent);
			break;
		case FIGHT:
			fight(player, opponent);
			break;
		case SURVIVE:
			survive(player);
			break;
		case HOLD:
			break;
		case DEAD:
			(*maze)[row][col] = SPACE;
			break;
		default: // DEAD
			break;
		}
	}
	else {
		support(player);
	}
}
/// <summary>
/// players disapear after first step.
///	loop starts without support action, need to start from 0.
/// </summary>
/// <param name="player"></param>
/// <param name="opponent"></param>
void Arena::walk(Player* player, Player* opponent) {
	startAStar = true;
	Cell* oldCell = player->getCell();
	pq.push(oldCell);
	grays.push_back(oldCell);
	Cell* newCell = nullptr;
	while (startAStar) {
		newCell = AStarIteration(opponent->getCell(), opponent->getTeam());
	}
	clearCollections();
	if (newCell == nullptr) {
		(*maze)[oldCell->getRow()][oldCell->getCol()] = player->getTeam();
		return;
	}
	(*maze)[oldCell->getRow()][oldCell->getCol()] = SPACE;
	(*maze)[newCell->getRow()][newCell->getCol()] = player->getTeam();
	newCell->reset();
	player->setCell(newCell);
}

void Arena::walk(Player* support, Crate* crate) {
	startAStar = true;
	Cell* oldCell = support->getCell();
	pq.push(oldCell);
	grays.push_back(oldCell);
	Cell* newCell = nullptr;
	Cell* crateCell = crate->getCell();
	while (startAStar) {
		newCell = AStarIteration(crateCell, crate->getType());
	}
	clearCollections();
	if (newCell == nullptr) {
		(*maze)[oldCell->getRow()][oldCell->getCol()] = support->getTeam();
		return;
	}
	(*maze)[oldCell->getRow()][oldCell->getCol()] = SPACE;
	(*maze)[newCell->getRow()][newCell->getCol()] = support->getTeam();
	newCell->reset();
	support->setCell(newCell);
}

void Arena::fight(Player* player, Player* opponent) {
	if (player->getBullets().size() > 0)
	{
		return;
	}
	if (player->getBulletAmmo() > 0)
	{
		int playerX = player->getCell()->getCol();
		int playerY = player->getCell()->getRow();
		int opponentX = opponent->getCell()->getCol();
		int opponentY = opponent->getCell()->getRow();
		double rad = atan2(opponentY - playerY, opponentX - playerX);
		Bullet* b = new Bullet(playerX + 0.5, playerY + 0.5, rad);
		b->setIsFired(true);
		vector<Bullet*> bullets = player->getBullets();
		bullets.push_back(b);
		player->setBullets(bullets);
		player->fireBullet();
	}
}

void Arena::survive(Player* player) {
	if (player->getTeam() == TEAM1)
	{
		walk(player, team1[0]);
	}
	else
	{
		walk(player, team2[0]);
	}
}

void Arena::support(Player* support) {
	if (support->getTeam() == TEAM1)
	{
		supportIfNeeded(team1);
	}
	else
	{
		supportIfNeeded(team2);
	}
	// if support is needed use A* with ammo, health, or player targets
}

void Arena::supportIfNeeded(vector<Player*> team) {
	Player* support = team[0];
	Player* player = nullptr;
	int supportType;
	Crate* crate = nullptr;
	for (size_t i = 1; i < team.size(); i++)
	{
		//check on friends - first player that needs support breaks loop
		player = team[i];
		if (player->getHealth() != 0)
		{
			supportType = player->getSupportType();
			if (supportType == HEALTH)
			{
				if (support->getCrateType() != HEALTH)
				{
					int index = findCrate(support, crates, HEALTH);
					crate = crates.at(index);
					walk(support, crate);
					if (support->getCell()->ManhattanDistance(crate->getCell()->getRow(), crate->getCell()->getCol()) <= 2)
					{
						support->setCrateType(HEALTH);
						cleanCrate(index);
					}
				}
				else {
					walk(support, player);
					if (support->getCell()->ManhattanDistance(player->getCell()->getRow(), player->getCell()->getCol()) <= 2)
					{
						player->heal();
						support->setCrateType(-1);
					}
				}
			}
			if (supportType == AMMO)
			{
				if (support->getCrateType() != AMMO)
				{
					int index = findCrate(support, crates, AMMO);
					crate = crates.at(index);
					walk(support, crate);
					if (support->getCell()->ManhattanDistance(crate->getCell()->getRow(), crate->getCell()->getCol()) <= 2)
					{
						support->setCrateType(AMMO);
						cleanCrate(index);
					}
				}
				else {
					walk(support, player);
					if (support->getCell()->ManhattanDistance(player->getCell()->getRow(), player->getCell()->getCol()) <= 2)
					{
						player->restockAmmo();
						support->setCrateType(-1);
					}
				}
			}
		}
	}
}

int Arena::findCrate(Player* support, vector<Crate*> crates, int type) {
	Cell* supportCell = support->getCell();
	int index = -1;
	Crate* crate = nullptr;
	double minManhattan = LONG_MAX;
	double newManhattan = LONG_MAX;
	for (size_t i = 0; i < crates.size(); i++)
	{
		if (crate == nullptr)
		{
			index = i;
			crate = crates[i];
			minManhattan = supportCell->ManhattanDistance(crate->getCell()->getRow(), crate->getCell()->getCol());
		}
		newManhattan = supportCell->ManhattanDistance(crates[i]->getCell()->getRow(), crates[i]->getCell()->getCol());
		if (newManhattan < minManhattan)
		{
			minManhattan = newManhattan;
			crate = crates[i];
			index = i;
		}

	}
	return index;
}

void Arena::cleanCrate(int index) {
	Cell* cell = crates.at(index)->getCell();
	(*maze)[cell->getRow()][cell->getCol()] = SPACE;
	crates.erase(crates.begin() + index);
}

/// <summary>
/// Impove line of fire to be centered
/// </summary>
/// <param name="player"></param>
/// <param name="opponent"></param>
void Arena::moveBullets(Player* player, Player* opponent) {
	vector<Bullet*> bullets = player->getBullets();

	for (size_t i = 0; i < bullets.size(); i++)
	{
		Bullet* b = bullets.at(i);
		b->move(*maze);
		b->show(player->getTeam());
		bool hitX = (int)(b->getX()) == opponent->getCell()->getCol();
		bool hitY = (int)(b->getY()) == opponent->getCell()->getRow();
		if (hitX && hitY)
		{
			opponent->takeDamage(1);
		}
		if (!b->getIsFired())
		{
			bullets.erase(bullets.begin() + i);
			i--;
		}
	}
	player->setBullets(bullets);
}

Cell* Arena::AStarIteration(Cell* target, int targetColor) {
	// Check if priority queue is not empty
	if (pq.empty())
	{
		startAStar = false;
		return nullptr;
	}
	Cell* newPosition = nullptr;
	Cell* pCurrent = pq.top();
	pq.pop();
	int row = pCurrent->getRow();
	int col = pCurrent->getCol();
	(*maze)[row][col] = BLACK;

	// Check the neighbors of pCurrent and pick the white ones to add to the priority queue
	// UP
	if (startAStar) {// can be diplicate targetcolor
		if (row + 1 < MSZ && ((*maze)[row + 1][col] == SPACE || (*maze)[row + 1][col] == targetColor)) {
			newPosition = CheckNeighbor(pCurrent, row + 1, col, target, targetColor);
		}
	}
	// DOWN
	if (startAStar) {
		if (row - 1 >= 0 && ((*maze)[row - 1][col] == SPACE || (*maze)[row - 1][col] == targetColor)) {
			newPosition = CheckNeighbor(pCurrent, row - 1, col, target, targetColor);
		}
	}
	// RIGHT
	if (startAStar) {
		if (col + 1 < MSZ && ((*maze)[row][col + 1] == SPACE || (*maze)[row][col + 1] == targetColor)) {
			newPosition = CheckNeighbor(pCurrent, row, col + 1, target, targetColor);
		}
	}
	// LEFT
	if (startAStar) {
		if (col - 1 >= 0 && ((*maze)[row][col - 1] == SPACE || (*maze)[row][col - 1] == targetColor)) {
			newPosition = CheckNeighbor(pCurrent, row, col - 1, target, targetColor);
		}
	}
	return newPosition;
}

// Row and col belong to the neighboring cells
Cell* Arena::CheckNeighbor(Cell* pCurrent, int row, int col, Cell* target, int targetColor) {
	if ((*maze)[row][col] == targetColor) // Only Monsters enter here, Pacman handles stopping condition in pacmanStep function
	{
		startAStar = false;
		return RestorePath(pCurrent);
	}
	else
	{
		(*maze)[row][col] = GRAY;
		Cell* c = new Cell(row, col, pCurrent);
		// Pacman is using the negative distance to get the farthest point in the top of the pq.
		// Then, summing with the function G which is the number of cells to travel in the path.
		c->computeH(target);
		//if (pacmanTurn)
		//{
		//	c->setH(-(c->getH()));
		//}
		c->setG(pCurrent->getG() + 1 + (*security_map)[row][col]);
		c->setF();
		pq.push(c);
		grays.push_back(c);
		return nullptr;
	}
}

// Return the starting position adjacent cell
Cell* Arena::RestorePath(Cell* pc)
{
	Cell* childCell = nullptr;
	while (pc->getParent() != nullptr)
	{
		childCell = pc;
		pc = pc->getParent();
	}
	return childCell;
}

void Arena::clearCollections() {
	Cell* temp;
	// Clearing the maze from blacks and grays
	for (unsigned i = 0; i < grays.size(); i++)
	{
		temp = grays.at(i);
		(*maze)[temp->getRow()][temp->getCol()] = SPACE;
	}
	grays.clear();
	// Emptying priority queue from cells
	while (!pq.empty()) {
		pq.pop();
	}
}