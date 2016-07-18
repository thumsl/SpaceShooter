#include "player.h"

player::player(float xPos, float yPos, int lValue, int hValue, float sValue) {
	x = initialX = xPos;
	y = initialY = yPos;
	lenght = lValue;
	height = hValue;
	speed = sValue;
	lookingAt = 3;
	health = 10;
	base_health = 10;
}

void player::mov(float xInc, float yInc) {
	x += xInc;
	y += yInc;
}

void player::resetPos() {
	x = initialX;
	y = initialY;
}

void player::turn(int direction) {
	lookingAt = direction;
}

void player::takeDamage(float d) {
	health -= d;
}

void player::setHealth(float h) {
	health = h;
}