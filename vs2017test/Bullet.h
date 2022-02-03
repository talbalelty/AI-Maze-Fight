#pragma once

#include "Definitions.h"

class Bullet
{
private:
	double x, y;
	double angle_direction;
	bool is_fired;
public:
	Bullet();
	Bullet(double xx, double yy);
	Bullet(double xx, double yy, double angle);
	~Bullet();
	void move(int maze[MSZ][MSZ]);
	void setIsFired(bool status) { is_fired = status; }
	void show();
	bool getIsFired() { return is_fired; }
	void SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage);
	
};

