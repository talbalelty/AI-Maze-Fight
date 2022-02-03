#pragma once

void initTeams(Room* rooms[NUM_ROOMS], int(*maze)[MSZ][MSZ]);
void initCrates(Room* rooms[NUM_ROOMS], int(*maze)[MSZ][MSZ]);
void findOpponents();
void iteration();
void playerStance(Player* player, Player* opponent);
