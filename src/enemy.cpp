#include "enemy.h"

enemy::enemy(int lValue, int hValue, float sValue, float h) {
	lenght = lValue;
	height = hValue;
	speed = sValue;
	health = h;
	base_health = h;
	right = left = up = down = false;
	timer = rand()%30 + 20;
	attack = 2;
}

void enemy::setPos(float xValue, float yValue) {
	x = xValue;
	y = yValue;
}

void enemy::takeDamage(float d) {
	health -= d;
}

void enemy::randomMov() {
	int direction;
	if (timer > 0)
		timer--;
	else {
		direction = rand()%6;
		if (direction == 0) {
			right = true;
			lookingAt = 3;
			left = up = down = false;
		}

		else if (direction == 1) {
			lookingAt = 2;
			left = true;
			right = up = down = false;
		}

		else if (direction == 2) {
			lookingAt = 0;
			up = true;
			right = left = down = false;
		}

		else if (direction == 3) {
			lookingAt = 1;
			down = true;
			right = left = up = false;
		}
		timer = rand()%30 + 20;
	}

	if (right == true)
		x += speed;
	else if (left == true)
		x -= speed;
	else if (up == true)
		y -= speed;
	else if (down == true)
		y += speed;
}

void enemy::swapDirections(int d) {
	if (d == 0) { // horizontal
		if (left == true) {
			left = false;
			right = true;
		}
		else if (right == true) {
			left = true;
			right = false;
		}
	}

	if (d == 1) {
		if (up == true) {
			up = false;
			down = true;
		}
		else if (down == true) {
			down = false;
			up = true;
		}
	}

	if (right == true)
			x += speed;
	else if (left == true)
			x -= speed;
	else if (up == true)
			y -= speed;
	else if (down == true)
			y += speed;
}