#ifndef LEVEL_H
#define LEVEL_H
#include <iostream>
#include <fstream>
#include <math.h>

namespace level {
	extern int number_of_enemies, health_value, attack_value;

	int enemies();
	int health();
	int attack();
	int initLevel(int);
	int getLevelFromFile(int, int);
};

#endif