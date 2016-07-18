#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>

class player {
	// coordinates
	float x, initialX, y, initialY;
	int lookingAt;
	// dimensions
	unsigned short int lenght;
	unsigned short int height;
	// stats
	unsigned short int attack;
	unsigned short int defense;
	float health;
	float base_health;
	float speed;

	public:
		float xValue() {return x;}
		float yValue() {return y;}
		int lValue() {return lenght;}
		int hValue() {return height;}
		float getHealth() {return health;}
		float getBaseHealth() {return base_health;}
		int getAttack() {return attack;}
		int getDefense() {return defense;}
		int facing() {return lookingAt;}
		float getSpeed() {return speed;}
		
		void takeDamage(float);
		void mov(float, float);
		void turn(int);
		void setHealth(float);
		void resetPos();

		player(float, float, int, int, float);	
};

#endif