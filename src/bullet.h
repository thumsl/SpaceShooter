#ifndef BULLET_H
#define BULLET_H
#include <iostream>

class bullet {
	// coorginates
	float x;
	float y;
	// dimensions
	unsigned short int lenght;
	unsigned short int height;
	// stats
	float yVel;
	float xVel;
	float health;
	float speed;

	public:
		bullet(int, int, float, float);

		float xValue() {return x;}
		float yValue() {return y;}
		int lValue() {return lenght;}
		int hValue() {return height;}
		float getSpeed() {return speed;}
		float getxVel() {return xVel;}
		float getyVel() {return yVel;}
		float getHealth() {return health;}
		void setStats(int, int, float);
		void setPos(float, float);
		void setVel(float, float);
		void updatePos();
		void takeDamage(float);
		void mov(float, float);
		void swapDirections();
};

#endif