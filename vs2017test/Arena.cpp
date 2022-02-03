#include "glut.h"
#include <time.h>
#include <vector>
#include <queue>
#include <iostream>
#include "Cell.h"
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Definitions.h"
#include "Player.h"
#include "Arena.h"
#include "Crate.h"

using namespace std;

const int TEAM_SIZE = 3;

vector<Player*> team1;
vector<Player*> team2;
vector<Crate*> crates;
int opponents[3];

void initTeams(Room* rooms[NUM_ROOMS], int(*maze)[MSZ][MSZ]) {
	Room room1 = *rooms[(rand() % (NUM_ROOMS / 2)) + (NUM_ROOMS / 2)];
	Room room2 = *rooms[rand() % (NUM_ROOMS / 2)];
	int room1row = room1.getCenterRow(), room1col = room1.getCenterCol();
	int room2row = room2.getCenterRow(), room2col = room2.getCenterCol();

	//First player is support
	for (size_t i = 0; i < TEAM_SIZE; i++)
	{
		team1.push_back(new Player(new Cell(room1row + i, room1col + i), i != 0));
		(*maze)[room1row + i][room1col + i] = TEAM1;
		team2.push_back(new Player(new Cell(room2row + i, room2col + i), i != 0));
		(*maze)[room2row + i][room2col + i] = TEAM2;
	}
}

void initCrates(Room* rooms[NUM_ROOMS], int(*maze)[MSZ][MSZ]) {
	Room room;
	int row, col;

	for (size_t i = 0; i < NUM_ROOMS; i++)
	{
		room = *rooms[i];
		row = (room.getCenterRow() - room.getH() / 2) + (rand() % room.getH());
		col = (room.getCenterCol() - room.getW() / 2) + (rand() % room.getW());
		crates.push_back(new Crate(new Cell(row, col), true));
		(*maze)[row][col] = AMMO;
		row = (room.getCenterRow() - room.getH() / 2) + (rand() % room.getH());
		col = (room.getCenterCol() - room.getW() / 2) + (rand() % room.getW());
		crates.push_back(new Crate(new Cell(row, col), false));
		(*maze)[row][col] = HP;
	}
}

void findOpponents() {
	for (size_t i = 1; i < TEAM_SIZE; i++)
	{
		opponents[i] = rand() % TEAM_SIZE;
	}
}

void iteration() {
	// for TEAM_SIZE - each iteration calls playerStance for each player. 6 calls total.
}

void playerStance(Player* player, Player* opponent) {
	if (player->getIsFighter())
	{
		int state = player->getState(opponent);
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
		default: // DEAD
			break;
		}
	}
	else {
		support(player);
	}
}

void walk(Player* player, Player* opponent) {
	
}

void fight(Player* player, Player* opponent) {

}

void survive(Player* player) {

}

void support(Player* player) {
	// for team - checkIfSupportIsNeeded(team);
	// if support is needed use A* with ammo, health, or player targets
}

int checkIfSupportIsNeeded(vector<Player*> team) {
	for (size_t i = 0; i < team.size(); i++)
	{
		//check on friends - first player that needs support breaks loop
	}
	return DEAD;
}


// Return the starting position adjacent cell
Cell* RestorePath(Cell* pc)
{
	Cell* childCell = pc;
	while (pc->getParent() != nullptr)
	{
		childCell = pc;
		pc = pc->getParent();
	}
	return childCell;
}

// Row and col belong to the neighboring cells
Cell* CheckNeighbor(Cell* pCurrent, int row, int col, vector<Cell*> target) {
	if (maze[row][col] == PACMAN) // Only Monsters enter here, Pacman handles stopping condition in pacmanStep function
	{
		startAStar = false;
		return RestorePath(new Cell(row, col, pCurrent));
	}
	else
	{
		maze[row][col] = GRAY;
		Cell* c = new Cell(row, col, pCurrent);
		// Pacman is using the negative distance to get the farthest point in the top of the pq.
		// Then, summing with the function G which is the number of cells to travel in the path.
		c->computeH(target);
		if (pacmanTurn)
		{
			c->setH(-(c->getH()));
		}
		c->setG(pCurrent->getG() + 1);
		c->setF();
		pq.push(c);
		grays.push_back(c);
		return nullptr;
	}
}

Cell* AStarIteration(Cell* target) {
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
	maze[row][col] = BLACK;

	// Check the neighbors of pCurrent and pick the white ones to add to the priority queue
	// UP
	if (startAStar) {
		if (row + 1 < MSZ && (maze[row + 1][col] == SPACE || maze[row + 1][col] == PACMAN)) {
			newPosition = CheckNeighbor(pCurrent, row + 1, col, target);
		}
	}
	// DOWN
	if (startAStar) {
		if (row - 1 >= 0 && (maze[row - 1][col] == SPACE || maze[row - 1][col] == PACMAN)) {
			newPosition = CheckNeighbor(pCurrent, row - 1, col, target);
		}
	}
	// RIGHT
	if (startAStar) {
		if (col + 1 < MSZ && (maze[row][col + 1] == SPACE || maze[row][col + 1] == PACMAN)) {
			newPosition = CheckNeighbor(pCurrent, row, col + 1, target);
		}
	}
	// LEFT
	if (startAStar) {
		if (col - 1 >= 0 && (maze[row][col - 1] == SPACE || maze[row][col - 1] == PACMAN)) {
			newPosition = CheckNeighbor(pCurrent, row, col - 1, target);
		}
	}
	return newPosition;
}

