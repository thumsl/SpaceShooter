#include "bullet.h"

bullet::bullet(int lValue, int hValue, float sValue, float h) {
	lenght = lValue;
	height = hValue;
	speed = sValue;
	health = h;
}

void bullet::updatePos() {
	x += xVel;
	y += yVel;
}

void bullet::setPos(float xValue, float yValue) {
	x += xValue;
	y += yValue;
}

void bullet::setVel(float xValue, float yValue) {
	xVel = xValue;
	yVel = yValue;
}

void bullet::setStats(int lValue, int hValue, float sValue) {
	lenght = lValue;
	height = hValue;
	speed = sValue;
}

void bullet::takeDamage(float d) {
	health -= d;
}

void bullet::mov(float xInc, float yInc) {
	x += xInc;
	y += yInc;
}

void bullet::swapDirections() {
	/*
	if ((*bullet)->b->getxVel()>0)
		(*bullet)->b->mov(1, 0);
	if ((*bullet)->b->getyVel()>0)
		(*bullet)->b->mov(0, 1);
	if ((*bullet)->b->getxVel()<0)
		(*bullet)->b->mov(-1, 0);
	if ((*bullet)->b->getyVel()<0)
		(*bullet)->b->mov(0, -1); */
	xVel = -xVel;
	yVel = -yVel;
}