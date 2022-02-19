#include "Bullet.h"
//#include <stdlib.h>
#include <math.h>
#include "glut.h"



Bullet::Bullet()
{
	angle_direction = (rand() % 360)*3.14 / 180; // in radians
	is_fired = false;
}

Bullet::Bullet(double xx, double yy)
{
	angle_direction = (rand() % 360)*3.14 / 180; // in radians
	is_fired = false;
	x = xx;
	y = yy;
}

Bullet::Bullet(double xx, double yy, double angle)
{
	angle_direction = angle; // in radians
	is_fired = false;
	x = xx;
	y = yy;
	double dx, dy;
	dx = cos(angle_direction);
	dy = sin(angle_direction);
	// update x,y
	x += dx;
	y += dy;
}

Bullet::~Bullet()
{
}

/// <summary>
/// TODO : figure out how characters take damage from bullets
/// </summary>
/// <param name="maze"></param>
void Bullet::move(int maze[MSZ][MSZ])
{
	// check that current x,y is in SPACE otherwise stop moving bullet
	int cellColor = maze[(int)(y)][(int)(x)];
	if (cellColor == WALL || cellColor == TEAM1 || cellColor == TEAM2) {
		is_fired = false;
	}
	if (is_fired)
	{
		double speed = 1.1;
		double dx, dy;
		dx = cos(angle_direction);
		dy = sin(angle_direction);
		// update x,y
		x += dx * speed;
		y += dy * speed;
	}
}

void Bullet::show(int playerColor)
{
	if (playerColor == TEAM1)
	{
		glColor3d(0, 1, 0); // green
	} 
	else {
		glColor3d(0, 1, 1); // yellow
	}
	glBegin(GL_POLYGON);
		glVertex2d(x, y + 0.5);
		glVertex2d(x+0.5, y );
		glVertex2d(x, y - 0.5);
		glVertex2d(x-0.5, y );
	glEnd();
	
}

void Bullet::SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage)
{
	while (is_fired) 
	{
		if (maze[(int)y][(int)x] == WALL)
			is_fired = false;
		else
		{
			security_map[(int)y][(int)x] += damage;
			double speed = 1;
			double dx, dy;
			dx = cos(angle_direction);
			dy = sin(angle_direction);
			// update x,y
			x += dx * speed;
			y += dy * speed;

		}

	}
}
