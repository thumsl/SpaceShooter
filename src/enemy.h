#ifndef ENEMY_H
#define ENEMY_H
#include <iostream>
#include <cstdlib>
#include <ctime>

class enemy {
	// coorginates
	float x;
	float y;
	// dimensions
	unsigned short int lenght;
	unsigned short int height;
	// stats
	float attack;
	float health;
	float base_health;
	float speed;
	// flags
	bool left, right, up, down, wait;
	int timer, lookingAt;

	public:
		enemy(int, int, float, float);
	 	float xValue() {return x;}
		float yValue() {return y;}
		int lValue() {return lenght;}
		int hValue() {return height;}
		int getAttack() {return attack;}
		float getSpeed() {return speed;}
		float getHealth() {return health;}
		float getBaseHealth() {return base_health;}
		int facing() {return lookingAt;}
		void setStats(int, int, float);
		void setPos(float, float);
		void takeDamage(float);
		void randomMov();	
		void swapDirections(int);
};

#endif