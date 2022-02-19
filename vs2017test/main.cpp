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
#include "Arena.h"

using namespace std;

Room* rooms[NUM_ROOMS];

int maze[MSZ][MSZ];
double security_map[MSZ][MSZ] = { 0 };

void InitMaze();
void InitRooms();
void DigTunnels();
void CreateSecurityMap();
Bullet* pb = nullptr;
Grenade* pg = nullptr;
Arena* arena;

void init()
{
	glClearColor(0.3, 0.3, 0.3, 0);// color of window background
	//           RED GREEN BLUE

	glOrtho(0, MSZ, 0, MSZ, -1, 1);
	//     left right bottom top near, far

	srand(time(0)); // initialize random seed

	InitMaze();
	InitRooms();
	DigTunnels();
	CreateSecurityMap();
	arena = new Arena(&maze, &rooms, &security_map);
}

void FillRoom(int index)
{
	int i, j;
	int cr, cc, w, h;

	w = rooms[index]->getW();
	h = rooms[index]->getH();
	cr = rooms[index]->getCenterRow();
	cc = rooms[index]->getCenterCol();


	for (i = cr - h / 2; i <= cr + h / 2; i++)
		for (j = cc - w / 2; j <= cc + w / 2; j++)
			maze[i][j] = SPACE;
}

bool hasOverlapping(int index, int w, int h, int crow, int ccol)
{
	int i;
	int dx, dy;
	bool overlap = false;
	for (i = 0; i < index && !overlap; i++)
	{
		dx = abs(ccol - rooms[i]->getCenterCol());
		dy = abs(crow - rooms[i]->getCenterRow());
		if (dx < w / 2 + rooms[i]->getW() / 2 + 3 &&
			dy < h / 2 + rooms[i]->getH() / 2 + 3)
			overlap = true;
	}
	return overlap;
}

/// <summary>
/// figure out how to fix obstacles and tunnels
/// </summary>
void InitRooms()
{
	int crow, ccol, w, h;
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		// init a room
		do {
			w = MIN_ROOM_WIDTH + rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH);
			h = MIN_ROOM_HEIGHT + rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT);
			crow = 2 + h / 2 + rand() % (MSZ - (5 + h));
			ccol = 2 + w / 2 + rand() % (MSZ - (5 + w));
		} while (hasOverlapping(i, w, h, crow, ccol));
		rooms[i] = new Room(crow, ccol, w, h);
		FillRoom(i);
	}

	// set random obstacles
	for (int i = 0; i < 200; i++)
		maze[rand() % MSZ][rand() % MSZ] = WALL;

}

void InitMaze()
{
	int i, j;
	// setup inner space of maze
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j] = WALL;


}

void RestorePath(Cell* pCurrent, int start_row, int start_col)
{

	//	while (!(current.getRow() == start_row && current.getCol() == start_col))
	while (pCurrent->getParent() != nullptr)
	{
		if (maze[pCurrent->getRow()][pCurrent->getCol()] == WALL) {
			maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
			maze[pCurrent->getRow()][pCurrent->getCol() + 1] = SPACE;
			maze[pCurrent->getRow() + 1][pCurrent->getCol()] = SPACE;
		}
		pCurrent = pCurrent->getParent();
	}
}


// who is the neighbor at nrow ncol? If it is white then paint it gray and add to pq
// If it is gray then check two cases: 
// 1. if F of this neighbor is now better then what was before then we need to update the neighbor
// 2. if it is not better then do nothing
// If it is black do nothing
// If it is Target then we have two cases (actually this is one of the previous cases, white or gray):
// 1. if F of this target is now better then what was before then we need to update the target
// 2. if it is not better then do nothing
void CheckNeighbor(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCells>& pq,
	vector <Cell>& grays, vector <Cell>& blacks)
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	double space_cost = 0.1, wall_cost = 2, cost;
	if (maze[nrow][ncol] == SPACE)
		cost = space_cost;
	else cost = wall_cost;
	// this is the neighbor
	Cell* pn = new Cell(nrow, ncol, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
		pCurrent->getG() + cost, pCurrent);
	// check the color of this neighbor
	it_black = find(blacks.begin(), blacks.end(), *pn);
	if (it_black != blacks.end()) // it was found i.e. it is black
		return;
	// white
	it_gray = find(grays.begin(), grays.end(), *pn);
	if (it_gray == grays.end()) // it wasn't found => it is white
	{
		grays.push_back(*pn); // paint it gray
		pq.push(*pn);
	}
	else // it is gray
	{
		//   new F       ?      old F
		if (pn->getF() < it_gray->getF()) // then update it (F of neighbor)!!!
		{
			// we need toupdate it in two places:
			// 1. in vector grays
			it_gray->setG(pn->getG());
			it_gray->setF(pn->getF());
			// 2. in PQ
			// to update a Cell in pq we need to remove it from pq, to update it and to push it back
			vector<Cell> tmp;
			Cell tmpCell;
			do
			{
				if (pq.empty())
				{
					cout << "ERROR! PQ is empty in update PQ\n";
					exit(1);
				}
				tmpCell = pq.top();
				pq.pop();
				if (!(tmpCell == (*pn))) // do not copy the Cell we were looking for to tmp!(see line 173) 
					tmp.push_back(tmpCell);
			} while (!(tmpCell == (*pn)));
			// now we are aout of do-while because we have found the neighbor in PQ. So change it to *pn.
			pq.push(*pn);
			// now push back all the elements that are in tmp
			while (!tmp.empty())
			{
				pq.push(tmp.back());
				tmp.pop_back();
			}
		}
	}
}

// runs A* from room i to room j
void DigPath(int i, int j)
{
	int row, col;
	vector <Cell> grays;
	vector <Cell> blacks;
	priority_queue <Cell, vector<Cell>, CompareCells> pq;
	Cell* pstart = new Cell(rooms[i]->getCenterRow(), rooms[i]->getCenterCol(),
		rooms[j]->getCenterRow(), rooms[j]->getCenterCol(), 0, nullptr);

	Cell* pCurrent;
	bool targetFound = false;
	// initializes grays and pq
	grays.push_back(*pstart);
	pq.push(*pstart);
	vector<Cell>::iterator it_gray;

	while (!pq.empty())
	{
		pCurrent = new Cell(pq.top());
		pq.pop();
		// If current is actually a target then we stop A*
		if (pCurrent->getRow() == rooms[j]->getCenterRow() &&
			pCurrent->getCol() == rooms[j]->getCenterCol()) // then it is target
		{ //in this case there cannot be a better path to target!!!
			RestorePath(pCurrent, rooms[i]->getCenterRow(), rooms[i]->getCenterCol());
			return;
		}
		// paint current black
		blacks.push_back(*pCurrent);
		it_gray = find(grays.begin(), grays.end(), *pCurrent); // we have to define operator ==
		if (it_gray != grays.end()) // current was found
			grays.erase(it_gray); // and removed from grays
		// now check the neighbors of current
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		// try to go UP (row -1)
		if (row > 0) // we can go UP
			CheckNeighbor(pCurrent, row - 1, col, pq, grays, blacks);

		if (row < MSZ - 1) // DOWN
			CheckNeighbor(pCurrent, row + 1, col, pq, grays, blacks);

		if (col < MSZ - 1) // RIGHT
			CheckNeighbor(pCurrent, row, col + 1, pq, grays, blacks);

		if (col > 0)//LEFT
			CheckNeighbor(pCurrent, row, col - 1, pq, grays, blacks);

	}
}

void DigTunnels()
{
	int i, j;
	for (i = 0; i < NUM_ROOMS; i++)
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			DigPath(i, j); // A*
			cout << "The path from room " << i << " to room " << j << " has been digged\n";
		}
}

void ShowMaze()
{
	int i, j;
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0, 0, 0);// set color black
				break;
			case SPACE:
				//				glColor3d(1, 1, 1);// set color white
								// show security data
				glColor3d(1 - security_map[i][j], 1 - security_map[i][j], 1 - security_map[i][j]);
				break;
			case TEAM1:
				glColor3d(1, 0, 0);// set color red
				break;
			case TEAM2:
				glColor3d(0, 0, 1);// set color blue
				break;
			case AMMO:
				glColor3d(0, 1, 1);// set color yellow
				break;
			case HEALTH:
				glColor3d(0, 1, 0);// set color green
				break;
			} // switch
			// now show the cell of maze
			glBegin(GL_POLYGON);
			glVertex2d(j, i); // left-bottom vertex
			glVertex2d(j, i + 1); // left-top vertex
			glVertex2d(j + 1, i + 1); // right-top vertex
			glVertex2d(j + 1, i); // right-bottom vertex
			glEnd();
		}
}

void CreateSecurityMap()
{
	int num_simulations = 1000;
	double damage = 0.001;
	int x, y;

	for (int i = 0; i < num_simulations; i++)
	{
		x = rand() % MSZ;
		y = rand() % MSZ;
		Grenade* g = new Grenade(x, y);
		g->SimulateExplosion(maze, security_map, damage);
	}
}
//void display(void)
//{
//	static const double ticks_per_second = 60.0; // or whatever
//	static int last;
//	int now = glutGet(GLUT_ELAPSED_TIME);
//	int tick = (int)floor(now * ticks_per_second / 1000);
//	if (!last)
//		last = tick - 1;
//	for (; last < tick; last++)
//		update();
//	draw();
//	glutPostRedisplay();
//}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowMaze();

	// show bullet
	//if (pb != nullptr)
	//{
	//	pb->show();
	//}

	//// show grenade
	//if (pg != nullptr)
	//{
	//	pg->show();
	//}

	arena->show();

	glutSwapBuffers(); // show all
}



// runs all the time in the background
void idle()
{
	if (pb && pb->getIsFired())
		pb->move(maze);
	if (pg && pg->getIsExploded())
		pg->explode(maze);

	// if something iteration()

	arena->iteration();
	Sleep(200);
	glutPostRedisplay(); // indirect call to display
}

void menu(int choice)
{
	if (choice == 1) // fire bullet
	{
		pb->setIsFired(true);
	}
	else if (choice == 2) // explode grenade
	{
		pg->setIsExploded(true);
	}
	else if (choice == 3) //Create Security Map
	{
		CreateSecurityMap();
	}

}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// place bullet in position (x,y)
		double xx, yy;
		xx = MSZ * (x / 600.0);
		yy = MSZ * ((600 - y) / 600.0);

		//		pb = new Bullet(xx, yy);
		pg = new Grenade(xx, yy);

	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 1200);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Dungeon");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	// add menu
	glutCreateMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Fire bullet", 1);
	glutAddMenuEntry("Explode Grenade", 2);
	glutAddMenuEntry("Create Security Map", 3);



	init();

	glutMainLoop();
}