#ifndef ENGINE_H
#define ENGINE_H
#define NORTH 0
#define SOUTH 1
#define WEST 2
#define EAST 3
#define WIDTH 1280
#define HEIGHT 720
#define HEALTH_BAR_HEIGHT 6
#define HEALTH_BAR_GAP 2

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "player.h"
#include "bullet.h"
#include "level.h"
#include "enemy.h"

extern bool up, down, left, right, running,  attack, boss, playing, finished;
extern player hero;  //x, y, lenght, height, speed
extern int current_level, angle;


struct bullets;
struct enemies;
struct stars;

void init();
void draw();
void keyboardHandling();
void createBullet(bullets**, float, float, int);
void destroyBullet(bullets**);
void playerMovement();
void CollisionDetection_bullet(bullets**, enemies**);
void CollisionDetection_hero(bullets**);
void CollisionDetection_enemies(enemies**);
bool enemiesExist(enemies*);
void generateEnemies(enemies**, int);
void destroyBullet(enemies**, enemies**);
void menu();
void play();
void levelFinished(int);
void playerDied();
void bossFight();
void enemyAttack(enemy*, bullets**);
void resetPlayer();
void genStars();
void updateStarPos(stars*);
#endif
