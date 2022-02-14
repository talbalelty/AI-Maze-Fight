#pragma once
#include "Definitions.h"
#include "Crate.h"
#include "glut.h"
#include <time.h>
#include <vector>
#include <queue>
#include <iostream>
#include "Cell.h"
#include "Room.h"
#include "MyCompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Definitions.h"
#include "Player.h"
#include <math.h>

using namespace std;

class Arena {
private:
	int (*maze)[MSZ][MSZ];
	Room* (*rooms)[NUM_ROOMS];
	vector<Cell*> grays;
	priority_queue<Cell*, vector<Cell*>, MyCompareCells> pq;
	vector<Player*> team1;
	vector<Player*> team2;
	vector<Crate*> crates;
	int opponents[TEAM_SIZE];
	bool startAStar = false;
public:
	Arena(int(*_maze)[MSZ][MSZ], Room* (*_rooms)[NUM_ROOMS]);
	void initTeams();
	void initCrates();
	Player* findOpponent(Player* player, vector<Player*> team);
	void iteration();
	void playerAction(Player* player, Player* opponent);
	void walk(Player* player, Player* opponent);
	void fight(Player* player, Player* opponent);
	void survive(Player* player);
	void support(Player* player);
	int checkIfSupportIsNeeded(vector<Player*> team);
	Cell* AStarIteration(Cell* target, int targetColor);
	Cell* CheckNeighbor(Cell* pCurrent, int row, int col, Cell* target, int targetColor);
	Cell* RestorePath(Cell* pc);
	void clearCollections();
	void moveBullets(Player* player, Player* opponent);
};
