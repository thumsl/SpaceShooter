#include "engine.h"

int current_level = 1, angle = 0, bossSong = 0;
bool up = false, down = false, left = false, right = false,
	 running = true, attack = false, boss = false, playing = false, finished = false, playBossSong = false; 
player hero(20.0, 300.0, 50, 50, 8.0); //postion xy, dimensions LxH, speed

Mix_Music *music = Mix_LoadMUS("data/music/s1.ogg");

typedef struct bullets {
// data structure of bullets	
	bullet* b;
	struct bullets* next;
	struct bullets* prev;
} bullets;

typedef struct enemies {
	enemy* e;
	struct enemies* next;
	struct enemies* prev;
} enemies;

typedef struct stars {
	int x;
	int y;
} stars;

stars starsArr[100];

void init() {
// this func. will set up the basic stuff needed to use OpenGL	
	// set an OpenGL window attribute before window creation. 
	// for colors, number of bits for that buffer channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// set where stuff is going to be drawn: up left corner = 0, 0; down right corner = WIDTH, HEIGHT;
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	// not needed for 2d
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	std::cout << "OpenGL running." << std::endl;
  	glAlphaFunc( GL_GREATER, 0 );							
  	glEnable( GL_ALPHA_TEST );	
	TTF_Init();

	Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2250);
	srand (time(NULL));
}

void keyboardHandling() {
// set the needed flags based on events that are registered
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {  // Closes the program if the X button on the window is clicked		
			running = false;
			playing = false;
		}

		if(e.type == SDL_MOUSEBUTTONDOWN)
			if (e.button.button == SDL_BUTTON_LEFT)
				if (playing == true)
					attack = true;

		if (e.type == SDL_KEYDOWN) {
			switch(e.key.keysym.sym) {
				case (SDLK_ESCAPE):
					if (playing == false)
							running = false;
					else
						playing = false;
					break;
				case (SDLK_w):
					up = true;
					break;
				case (SDLK_s):
					down = true;
					break;
				case (SDLK_a):
					left = true;
					break;
				case (SDLK_d):
					right = true;
					break;
				case (SDLK_p):
					playing = true;
					break;
			}
		}
		else if (e.type == SDL_KEYUP) {
			switch(e.key.keysym.sym) {
				case (SDLK_w):
					up = false;
					break;
				case (SDLK_s):
					down = false;
					break;
				case (SDLK_a):
					left = false;
					break;
				case (SDLK_d):
					right = false;
					break;
			}
		}
	}
}

void createBullet(bullets** bulletsList, float x, float y, int facing) {
// generate a new bullet, appearing on the right position (utilizing the hero's position)
	if (*bulletsList == NULL) {
		*bulletsList = (bullets*)malloc(sizeof(bullets));
		(*bulletsList)->b = new bullet(6, 6, 10.0, 10.0); //lenght, height, speed, health
		(*bulletsList)->next = NULL;
		(*bulletsList)->prev = NULL;
		switch(facing) {
			case (NORTH):
				(*bulletsList)->b->setPos(x+(hero.lValue()/2), y-10);
				(*bulletsList)->b->setVel(0, -(*bulletsList)->b->getSpeed());
				break;
			case (SOUTH):
				(*bulletsList)->b->setPos(x+(hero.lValue()/2), y+hero.hValue()+5);
				(*bulletsList)->b->setVel(0, (*bulletsList)->b->getSpeed());
				break;
			case (WEST):
				(*bulletsList)->b->setPos(x-10, y+(hero.hValue()/2));
				(*bulletsList)->b->setVel(-(*bulletsList)->b->getSpeed(), 0);
				break;
			case (EAST):
				(*bulletsList)->b->setPos(x+hero.lValue()+5, y+(hero.hValue()/2));
				(*bulletsList)->b->setVel((*bulletsList)->b->getSpeed(), 0);
				break;
		}
	}
	else {
		bullets* aux = *bulletsList;
		bullets* prev;
		while (aux!=NULL) {
			prev = aux;
			aux = aux -> next;
		}
		aux = (bullets*)malloc(sizeof(bullets));
		aux -> b = new bullet(6, 6, 10.0, 10.0); //lenght, height, speed, health
		prev -> next = aux;
		aux -> next = NULL;
		aux -> prev = prev;
		switch(facing) {
			case (NORTH):
				aux->b->setPos(x+(hero.lValue()/2), y-10);
				aux->b->setVel(0, -aux->b->getSpeed());
				break;
			case (SOUTH):
				aux->b->setPos(x+(hero.lValue()/2), y+hero.hValue()+5);
				aux->b->setVel(0, aux->b->getSpeed());
				break;
			case (WEST):
				aux->b->setPos(x-10, y+(hero.hValue()/2));
				aux->b->setVel(-aux->b->getSpeed(), 0);
				break;
			case (EAST):
				aux->b->setPos(x+hero.lValue()+5, y+(hero.hValue()/2));
				aux->b->setVel(aux->b->getSpeed(), 0);
				break;
		}	
	}
	//attack = false;
}

void destroyBullet(bullets** bulletsList, bullets** target) {
// destroy a bullet once its health goes to 0 or below
	if (*target == *bulletsList)
		*bulletsList = (*target)->next;

	if ((*target)->next != NULL)
		(*target)->next->prev = (*target)->prev;

	if ((*target)->prev != NULL)
		(*target)->prev->next = (*target)->next;

	free (*target);
}

void playerMovement() {
// player movement part, adds the player's current speed to its position on the grid 
// also verify collision of the player with the scenario walls 
	if (left) {
		hero.mov(-hero.getSpeed(), 0); // movement in pixels, first x then y
		hero.turn(WEST);
		angle = 270;
	}
	if (right) {
		hero.mov(hero.getSpeed(), 0);
		hero.turn(EAST);
		angle = 90;
	}
	if (up) {
		hero.mov(0, -hero.getSpeed());
		hero.turn(NORTH);
		angle = 0;
	}
	if (down) {
		hero.mov(0, hero.getSpeed());
		hero.turn(SOUTH);
		angle = 180;
	}

	// verify if the player has passed the scenario borders;
	// if so, brings him back
	if (hero.xValue()+hero.lValue() > WIDTH)
		hero.mov(-(hero.xValue()+hero.lValue()-WIDTH), 0);
	if (hero.xValue()<0)
		hero.mov(-hero.xValue(), 0);
	if (hero.yValue()+hero.hValue() > HEIGHT)
		hero.mov(0, -(hero.yValue()+hero.hValue()-HEIGHT));
	if (hero.yValue()<0)
		hero.mov(0, -hero.yValue());
}

void CollisionDetection_bullet(bullets** bullet, enemies** enemiesList, enemy* final_boss) {
// detect collision for the bullets -> hero, walls, enemies
	// collision with the scenario walls
	if ((*bullet)->b->xValue()+(*bullet)->b->lValue() > WIDTH) {
		(*bullet)->b->setVel(-(*bullet)->b->getxVel(), 0);
		(*bullet)->b->takeDamage(5);
	}
	if ((*bullet)->b->xValue()<0) {
		(*bullet)->b->setVel(-(*bullet)->b->getxVel(), 0);
		(*bullet)->b->takeDamage(5);
	}
	if ((*bullet)->b->yValue()+(*bullet)->b->hValue() > HEIGHT) {
		(*bullet)->b->setVel(0, -(*bullet)->b->getyVel());
		(*bullet)->b->takeDamage(5);
	}
	if ((*bullet)->b->yValue()<0) {
		(*bullet)->b->setVel(0, -(*bullet)->b->getyVel());
		(*bullet)->b->takeDamage(5);
	}	

	// collision with the player
	if ((*bullet)->b->xValue() + (*bullet)->b->lValue() >= hero.xValue() &&
		(*bullet)->b->yValue() + (*bullet)->b->hValue() >= hero.yValue() &&
		(*bullet)->b->xValue() <= hero.xValue() + hero.lValue() &&
		(*bullet)->b->yValue() <= hero.yValue() + hero.hValue())
	{
		Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
		Mix_PlayChannel(-1, hit, 0);
		(*bullet)->b->swapDirections();
		(*bullet)->b->takeDamage(5);
		hero.takeDamage(1);
	}


	// collision with an enemy
	enemies* aux_enemy = *enemiesList;
	while (aux_enemy != NULL) {
		if ((*bullet)->b->xValue() + (*bullet)->b->lValue() >= aux_enemy->e->xValue() &&
			(*bullet)->b->yValue() + (*bullet)->b->hValue() >= aux_enemy->e->yValue() &&
			(*bullet)->b->xValue() <= aux_enemy->e->xValue() + aux_enemy->e->lValue() &&
			(*bullet)->b->yValue() <= aux_enemy->e->yValue() + aux_enemy->e->hValue())
		{
			Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
			Mix_PlayChannel(-1, hit, 0);
			aux_enemy->e->takeDamage(5);
			(*bullet)->b->swapDirections();
			(*bullet)->b->takeDamage(5);
		}
		aux_enemy = aux_enemy->next;
	}

	// collision wit the boss
	if (boss) {
		if ((*bullet)->b->xValue() + (*bullet)->b->lValue() >= final_boss->xValue() &&
			(*bullet)->b->yValue() + (*bullet)->b->hValue() >= final_boss->yValue() &&
			(*bullet)->b->xValue() <= final_boss->xValue() + final_boss->lValue() &&
			(*bullet)->b->yValue() <= final_boss->yValue() + final_boss->hValue())
		{
			Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
			Mix_PlayChannel(-1, hit, 0);
			final_boss->takeDamage(5);
			(*bullet)->b->swapDirections();
			(*bullet)->b->takeDamage(5);
		}
	}
}

void CollisionDetection_enemyBullet(bullets** bullet, enemies** enemiesList) {
// detect collision for the bullets -> hero, walls, enemies
	// collision with the scenario walls
	if ((*bullet)->b->xValue()+(*bullet)->b->lValue() > WIDTH) {
		(*bullet)->b->setVel(-(*bullet)->b->getxVel(), 0);
		(*bullet)->b->takeDamage(5);
	}
	if ((*bullet)->b->xValue()<0) {
		(*bullet)->b->setVel(-(*bullet)->b->getxVel(), 0);
		(*bullet)->b->takeDamage(5);
	}
	if ((*bullet)->b->yValue()+(*bullet)->b->hValue() > HEIGHT) {
		(*bullet)->b->setVel(0, -(*bullet)->b->getyVel());
		(*bullet)->b->takeDamage(5);
	}
	if ((*bullet)->b->yValue()<0) {
		(*bullet)->b->setVel(0, -(*bullet)->b->getyVel());
		(*bullet)->b->takeDamage(5);
	}	

	// collision with the player
	if ((*bullet)->b->xValue() + (*bullet)->b->lValue() >= hero.xValue() &&
		(*bullet)->b->yValue() + (*bullet)->b->hValue() >= hero.yValue() &&
		(*bullet)->b->xValue() <= hero.xValue() + hero.lValue() &&
		(*bullet)->b->yValue() <= hero.yValue() + hero.hValue())
	{
		Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
		Mix_PlayChannel(-1, hit, 0);
		(*bullet)->b->swapDirections();
		(*bullet)->b->takeDamage(5);
		hero.takeDamage(1);
	}
}

void CollisionDetection_hero(bullets** bullet, enemy* e) {
// detect collision for the hero
	// hero collision with the bullets
	if (left || right || up || down) {
		// collision with a bullet
		if (bullet != NULL)
			if (hero.xValue()+hero.lValue() >= (*bullet)->b->xValue() &&
				hero.yValue()+hero.hValue() >= (*bullet)->b->yValue() &&
				hero.xValue() <= (*bullet)->b->xValue()+(*bullet)->b->lValue() &&
				hero.yValue() <= (*bullet)->b->yValue()+(*bullet)->b->hValue()) {
					left = right = up = down = false;
					Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
					Mix_PlayChannel(-1, hit, 0);
			}

		// collision with a enemy
		if (e != NULL)
			if ((e->xValue() <= (hero.xValue() + hero.lValue())) &&
			((e->xValue() + e->lValue()) >= hero.xValue()) &&
			((e->yValue() + e->hValue()) >= hero.yValue()) &&
			(e->yValue() <= hero.yValue() + hero.hValue())) {
				left = right = up = down = false;
				Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
				Mix_PlayChannel(-1, hit, 0);
				left = right = up = down = false;
				e->swapDirections(0);
				e->swapDirections(1);
			}
	}
}

void CollisionDetection_enemies(enemy* e) {
// Collision with the scenario walls
	if (e->xValue()+e->lValue() > WIDTH || e->xValue() < 0)
		e->swapDirections(0);
	else if (e->yValue()+e->hValue() > HEIGHT || e->yValue() < 0)
		e->swapDirections(1);

	// Collision with the player
	if ((e->xValue() <= (hero.xValue() + hero.lValue())) &&
		((e->xValue() + e->lValue()) >= hero.xValue()) &&
		((e->yValue() + e->hValue()) >= hero.yValue()) &&
		(e->yValue() <= hero.yValue() + hero.hValue())) {
			e->swapDirections(0);
			e->swapDirections(1);
			e->takeDamage(1);
			hero.takeDamage(e->getAttack());
			Mix_Chunk *hit = Mix_LoadWAV("data/fx/player_hit.ogg" );
			Mix_PlayChannel(-1, hit, 0);
		}
}

bool enemiesExist(enemies* enemiesList) {
// returns true or false
	if (enemiesList != NULL)
		return true;
	else
		return false;
}

void enemyAttack(enemy* e, bullets** enemyBulletsList) {
// makes the enemy attack, randomly
	int x = rand() % 50;
	if (x == 42)
		createBullet(enemyBulletsList, e->xValue(), e->yValue(), e->facing());
}

void generateEnemies(enemies** enemiesList, int number_enemies) {
// should only be used when there is no enemy alive, will assume enemiesList is empty
	float yGap = ((HEIGHT*0.9)/number_enemies) - 30.0;
	float prev_y_value = yGap;
	float x = 0.9*WIDTH;

	*enemiesList = (enemies*)malloc(sizeof(enemies));
	(*enemiesList)->e = new enemy(30.0, 30.0, 4.0, level::health());
	(*enemiesList)->e->setPos(x, yGap);
	(*enemiesList)->prev = NULL;
	(*enemiesList)->next = NULL;
	prev_y_value += 30.0 + yGap;
	number_enemies--;

	enemies* prev = *enemiesList;
	enemies* current = (*enemiesList)->next;
	for (int i = 0; i < number_enemies; i++) {
		current = (enemies*)malloc(sizeof(enemies));
		current->e = new enemy(30.0, 30.0, 4.0, level::health());
		current->e->setPos(x, prev_y_value);
		prev->next = current;
		current->prev = prev;
		current->next = NULL;
		prev = current;
		current = current->next;
		prev_y_value += 30.0 + yGap;
	}
}

void destroyEnemy(enemies** enemiesList, enemies** target) {
// destroy an enemy once its health goes to 0 or below
	if (*target == *enemiesList)
		*enemiesList = (*target)->next;

	if ((*target)->next != NULL)
		(*target)->next->prev = (*target)->prev;

	if ((*target)->prev != NULL)
		(*target)->prev->next = (*target)->next;

	free (*target);	

	Mix_Chunk *hit = Mix_LoadWAV("data/fx/explosion.wav" );
	Mix_PlayChannel(-1, hit, 0);
}

//Function for loading an image into an texture
GLuint loadTexture(const std::string &fileName) {
	SDL_Surface *image = IMG_Load(fileName.c_str()); // loads image to SDL surface
	SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB4444, 0);
	unsigned object(0); // texture
	glGenTextures(1, &object); // create texture from "object"
	glBindTexture(GL_TEXTURE_2D, object); // bind the texture to GL_TEXTURE_2D
	// functions to create texture out of the SDL_Surface
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repeat on X axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);  // Stretch on Y axis
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image);
	return object;
}

void menu() {
// draws the menu while playing != true
	SDL_Window *menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	300, 300, SDL_WINDOW_OPENGL); // | SDL_WINDOW_FULLSCREEN_DESKTOP
	SDL_Renderer * renderer = SDL_CreateRenderer(menuWindow, -1, 0);

	if (music == NULL)
		std::cout << Mix_GetError() << std::endl;
	Mix_PlayMusic(music, -1);

	TTF_Font *sixty = TTF_OpenFont("data/misc/sixty.ttf", 50);
	TTF_Font *arial = TTF_OpenFont("data/misc/arial.ttf", 14);
	SDL_Color playColor = {255, 255, 255};
	SDL_Color color = {255, 255, 255};

	int playW = 0, playH = 0;
	SDL_Surface *playSurface = TTF_RenderText_Solid(sixty, "Play", playColor);
	SDL_Texture *playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
	SDL_QueryTexture(playTexture, NULL, NULL, &playW, &playH);
	SDL_Rect playRect = {(300-playW)/2, 40, playW, playH};

	int creditsW = 0, creditsH = 0;
	SDL_Surface *creditsSurface = TTF_RenderText_Solid(sixty, "Credits", color);
	SDL_Texture *creditsTexture = SDL_CreateTextureFromSurface(renderer, creditsSurface);
	SDL_QueryTexture(creditsTexture, NULL, NULL, &creditsW, &creditsH);
	SDL_Rect creditsRect = {(300-creditsW)/2, 40+playH+40, creditsW, creditsH};

	int exitW = 0, exitH = 0;
	SDL_Surface *exitSurface = TTF_RenderText_Solid(arial, "Press Escape to exit.", color);
	SDL_Texture *exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);
	SDL_QueryTexture(exitTexture, NULL, NULL, &exitW, &exitH);
	SDL_Rect exitRect = {(300-exitW)/2, 80+playH+creditsH+30, exitW, exitH};

	SDL_Event e;
	int x = 0, y = 0;
	while (!playing && running) {
		// keyboardHandling();
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {  // Closes the program if the X button on the window is clicked		
				running = false;
				playing = false;
			}

			if (e.type == SDL_MOUSEBUTTONDOWN)
				if (e.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&x, &y);
					if ((x >= (300-playW)/2 && x <= (300-playW)/2+playW) &&
						(y >= 40 && y <= 40+playH))
						playing = true;
				}

			if (e.type == SDL_KEYDOWN)
				if (e.key.keysym.sym == SDLK_ESCAPE)
					running = false;
		}

		SDL_GetMouseState(&x, &y);
		if ((x >= (300-playW)/2 && x <= (300-playW)/2+playW) && (y >= 40 && y <= 40+playH)) {
			playColor = {255, 0, 0};
			SDL_DestroyTexture(playTexture);
			SDL_FreeSurface(playSurface);
			playSurface = TTF_RenderText_Solid(sixty, "Play", playColor);
			playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
		}
		else {
			playColor = {255, 255, 255};
			SDL_DestroyTexture(playTexture);
			SDL_FreeSurface(playSurface);
			playSurface = TTF_RenderText_Solid(sixty, "Play", playColor);
			playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
		}

		SDL_RenderCopy(renderer, playTexture, NULL, &playRect);
		SDL_RenderCopy(renderer, creditsTexture, NULL, &creditsRect);
		SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(playTexture);
	SDL_DestroyTexture(creditsTexture);
	SDL_DestroyTexture(exitTexture);
	SDL_FreeSurface(playSurface);
	SDL_FreeSurface(creditsSurface);
	SDL_FreeSurface(exitSurface);
	TTF_CloseFont(sixty);
	TTF_CloseFont(arial);
	SDL_DestroyWindow(menuWindow);
}

void play() {
	// lists of entities
	enemies* enemiesList = NULL;	// list of enemies;
	bullets* bulletsList = NULL;	// list of bullets to be rendered
	bullets* enemyBulletsList = NULL; 
	bullets* aux; // content will vary depending on the current level (read from file)
	bullets* prevBullet = NULL;

	SDL_Window *screen = SDL_CreateWindow("game title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL); // | SDL_WINDOW_FULLSCREEN_DESKTOP
	SDL_GLContext context = SDL_GL_CreateContext(screen);

	unsigned int player_texture = 0, enemy_texture = 0, boss_texture = 0;
	player_texture = loadTexture("data/sprites/hero.png");
	enemy_texture = loadTexture("data/sprites/enemy1.png");
	boss_texture = loadTexture("data/sprites/boss.png");

	int bossAngle = 0;
	level::initLevel(current_level);
	generateEnemies(&enemiesList, level::enemies());
	genStars();

	enemy* final_boss = new enemy(100, 100, 10.0, 100);
	final_boss->setPos(600, 300);

	std::cout << "Game loop started." << std::endl;	
	while (playing && running && !finished) {
		glLoadIdentity();

		if (!boss) {
			if (!enemiesExist(enemiesList)) {
				if (level::initLevel(++current_level) == -1) {
					
					boss = true;
					finished = true;
					resetPlayer();
				}
				else {
					levelFinished(current_level-1);
					finished = true;
					playBossSong = false;
					resetPlayer();
				}
			}
		}
		
		if (bossSong == 0 && playBossSong == true) {
			music = Mix_LoadMUS("data/music/s2.ogg");
			Mix_PlayMusic(music, -1);
		}

		// **EVENT HANDLING**
		keyboardHandling();
		
		// **LOGICS**
		if (hero.getHealth() <= 0) {
			playing = false;
			if (boss = true) {
				boss = false;
			}
			current_level = 1;
			playerDied();
			resetPlayer();
		}

		playerMovement();
		if (attack) { // shots fired?
			createBullet(&bulletsList, hero.xValue(), hero.yValue(), hero.facing());
			attack = false;
		}

		// **RENDERING**
		glClear(GL_COLOR_BUFFER_BIT); 
		glClearColor(0, 0, 0, 0); // red, green, blue, alpha 

		glPushMatrix(); // start rendering
		glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
		glColor4ub(255, 255, 0, 255);
		// renders the stars
		for (int i = 0; i < 100; i++) {
			glBegin(GL_QUADS);
				glTexCoord2d(0,0); glVertex2f(starsArr[i].x, starsArr[i].y);
				glTexCoord2d(1,0); glVertex2f(starsArr[i].x+1, starsArr[i].y);
				glTexCoord2d(1,1); glVertex2f(starsArr[i].x+1, starsArr[i].y+1);
				glTexCoord2d(0,1); glVertex2f(starsArr[i].x, starsArr[i].y+1);
			glEnd();				
			updateStarPos(&starsArr[i]);
		}
		glPopMatrix();
		
		// draws the player		
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, player_texture);
		glTranslatef(hero.xValue() + hero.lValue()/2, hero.yValue() + hero.hValue()/2, 0);
		glRotatef((float)angle, 0, 0, 1);
		glBegin(GL_QUADS);
			glColor4ub(255, 255, 255, 255);
			glTexCoord2d(0,0); glVertex2f(-(hero.lValue()/2), -(hero.hValue()/2));
			glTexCoord2d(1,0); glVertex2f(hero.lValue()/2, -(hero.hValue()/2));
			glTexCoord2d(1,1); glVertex2f(hero.lValue()/2, hero.hValue()/2);
			glTexCoord2d(0,1); glVertex2f(-(hero.lValue()/2), hero.hValue()/2);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glPushMatrix();
		glOrtho(0, WIDTH, HEIGHT, 0, -1, 1); // set the matrix size
		// draws all the bullets and updates their position
		aux = bulletsList;
		while(aux != NULL) { // go through the bullet list, drawing each of them on the screen
			if (aux->b->getHealth()>0) {
				// ***collision detection*** ///
				CollisionDetection_bullet(&aux, &enemiesList, final_boss);
				CollisionDetection_hero(&aux, NULL);

				// draw the bullets
				glBegin(GL_QUADS);
					glVertex2f(aux->b->xValue(), aux->b->yValue());
					glVertex2f(aux->b->xValue()+aux->b->lValue(), aux->b->yValue());
					glVertex2f(aux->b->xValue()+aux->b->lValue(), aux->b->yValue()+aux->b->hValue());
					glVertex2f(aux->b->xValue(), aux->b->yValue()+aux->b->hValue());
				glEnd();
				prevBullet = aux;
				aux->b->updatePos();
			}
			else
				destroyBullet(&bulletsList, &aux);
			if (aux!=NULL)
				aux = aux->next;
		}

		glColor4ub(255, 0, 50, 255);
		aux = enemyBulletsList;
		while(aux != NULL) { // go through the bullet list, drawing each of them on the screen
			if (aux->b->getHealth()>0) {
				// ***collision detection*** ///
				CollisionDetection_enemyBullet(&aux, &enemiesList);
				CollisionDetection_hero(&aux, NULL);
				// draw the bullets
				glBegin(GL_QUADS);
					glVertex2f(aux->b->xValue(), aux->b->yValue());
					glVertex2f(aux->b->xValue()+aux->b->lValue(), aux->b->yValue());
					glVertex2f(aux->b->xValue()+aux->b->lValue(), aux->b->yValue()+aux->b->hValue());
					glVertex2f(aux->b->xValue(), aux->b->yValue()+aux->b->hValue());
				glEnd();
				prevBullet = aux;
				aux->b->updatePos();
			}
			else
				destroyBullet(&enemyBulletsList, &aux);
			if (aux!=NULL)
				aux = aux->next;
		}		

		// draws the player's health bar
		int health_bar_lenght = (hero.lValue()*hero.getHealth())/hero.getBaseHealth(); // in pixels
		if (health_bar_lenght < 0)
			health_bar_lenght = 0;
		glBegin(GL_QUADS);
			glColor4ub(255, 0, 0, 255);	
			glVertex2f(hero.xValue(), hero.yValue()-HEALTH_BAR_HEIGHT);
			glVertex2f(hero.xValue() + health_bar_lenght, hero.yValue()-HEALTH_BAR_HEIGHT);
			glVertex2f(hero.xValue() + health_bar_lenght, hero.yValue()-(HEALTH_BAR_HEIGHT+HEALTH_BAR_GAP));
			glVertex2f(hero.xValue(), hero.yValue()-(HEALTH_BAR_HEIGHT+HEALTH_BAR_GAP));	
		glEnd();

		// draws the enemies
		enemies* aux_enemy = enemiesList;
		while (aux_enemy != NULL) { // go trhough the list of enemies and render all of them
			if (aux_enemy->e->getHealth() > 0) {
				// draw the health bar over the enemy
				CollisionDetection_hero(NULL, aux_enemy->e);
				enemyAttack(aux_enemy->e, &enemyBulletsList);
				int health_bar_lenght = (aux_enemy->e->lValue()*aux_enemy->e->getHealth())/aux_enemy->e->getBaseHealth();// in pixels
				if (health_bar_lenght < 0)
					health_bar_lenght = 0;
				glBegin(GL_QUADS);
					glColor4ub(255, 0, 0, 255); // red color for the health bar
					glVertex2f(aux_enemy->e->xValue(), aux_enemy->e->yValue()-10);
					glVertex2f(aux_enemy->e->xValue() + health_bar_lenght, aux_enemy->e->yValue()-10);
					glVertex2f(aux_enemy->e->xValue() + health_bar_lenght, aux_enemy->e->yValue()-5);
					glVertex2f(aux_enemy->e->xValue(), aux_enemy->e->yValue()-5);	
				glEnd();

				//draw thee enemy itself
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, enemy_texture);
				glBegin(GL_QUADS);
					glColor4ub(255, 255, 255, 255);
					CollisionDetection_enemies(aux_enemy->e);
					glTexCoord2d(0,0); glVertex2f(aux_enemy->e->xValue(), aux_enemy->e->yValue());
					glTexCoord2d(1,0); glVertex2f(aux_enemy->e->xValue()+aux_enemy->e->lValue(), aux_enemy->e->yValue());
					glTexCoord2d(1,1); glVertex2f(aux_enemy->e->xValue()+aux_enemy->e->lValue(), aux_enemy->e->yValue()+aux_enemy->e->hValue());
					glTexCoord2d(0,1); glVertex2f(aux_enemy->e->xValue(), aux_enemy->e->yValue()+aux_enemy->e->hValue());	
				glEnd();
				glDisable(GL_TEXTURE_2D);
				aux_enemy->e->randomMov();
			}
			else
				destroyEnemy(&enemiesList, &aux_enemy);
			if (aux_enemy!=NULL)
				aux_enemy = aux_enemy->next;
		}
		glPopMatrix(); // end

		if (boss) {
			bossSong++;
			playBossSong = true;
			if (final_boss->getHealth() > 0) {
				health_bar_lenght = (final_boss->lValue()*final_boss->getHealth())/final_boss->getBaseHealth();
				if (health_bar_lenght < 0)
					health_bar_lenght = 0;
				glPushMatrix();
				glLoadIdentity();
				glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
				glBegin(GL_QUADS);
					glColor4ub(255, 0, 0, 255); // red color for the health bar
					glVertex2f(final_boss->xValue(),final_boss->yValue()-25);
					glVertex2f(final_boss->xValue() + health_bar_lenght, final_boss->yValue()-25);
					glVertex2f(final_boss->xValue() + health_bar_lenght, final_boss->yValue()-20);
					glVertex2f(final_boss->xValue(), final_boss->yValue()-20);
				glEnd();
				glPopMatrix();

				CollisionDetection_hero(NULL, final_boss);
				CollisionDetection_enemies(final_boss);
				enemyAttack(final_boss, &enemyBulletsList);

				glPushMatrix();
				glLoadIdentity();
				glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, boss_texture);
				glTranslatef(final_boss->xValue() + final_boss->lValue()/2, final_boss->yValue() + final_boss->hValue()/2, 0);
				glRotatef((float)bossAngle, 0, 0, 1);
					glBegin(GL_QUADS);
						glColor4ub(255, 255, 255, 255);
						glTexCoord2d(0,0); glVertex2f(-(final_boss->lValue()/2), -(final_boss->hValue()/2));
						glTexCoord2d(1,0); glVertex2f(final_boss->lValue()/2, -(final_boss->hValue()/2));
						glTexCoord2d(1,1); glVertex2f(final_boss->lValue()/2, final_boss->hValue()/2);
						glTexCoord2d(0,1); glVertex2f(-(final_boss->lValue()/2), final_boss->hValue()/2);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				final_boss->randomMov();
				bossAngle+=2;
			}
			else {
				free(final_boss);
				boss = false;
				playing = false;
				resetPlayer();
				bossSong = 0;
				playBossSong =  false;
				current_level = 1;
			}
		}
		glPopMatrix(); // end
		SDL_GL_SwapWindow(screen);
		SDL_Delay(12);
	}

	SDL_DestroyWindow(screen);
}

/*
void credtis() {
// render the level finishing message
	SDL_Event e;
	int key_pressed = SDLK_a;

	SDL_Window *menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	WIDTH, HEIGHT, SDL_WINDOW_OPENGL); // | SDL_WINDOW_FULLSCREEN_DESKTOP
	SDL_Renderer * renderer = SDL_CreateRenderer(menuWindow, -1, 0);

	TTF_Font *janvier = TTF_OpenFont("data/misc/24Janvier-Light", 50);
	TTF_Font *arial = TTF_OpenFont("data/misc/arial.ttf", 16);
	SDL_Color color = {255, 255, 255};

	int msg1W = 0, msg1H = 0;
	SDL_Surface *msg1Surface = TTF_RenderText_Solid(janvier, "", color);
	SDL_Texture *msg1Texture = SDL_CreateTextureFromSurface(renderer, msg1Surface);
	SDL_QueryTexture(msg1Texture, NULL, NULL, &msg1W, &msg1H);
	SDL_Rect msg1Rect = {(WIDTH-msg1W)/2, (HEIGHT-msg1H)/2, msg1W, msg1H};

	while (key_pressed != SDLK_ESCAPE) {
		SDL_PollEvent(&e);
		if (e.type == SDL_KEYDOWN)
			 key_pressed = e.key.keysym.sym;
		SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
		SDL_RenderCopy(renderer, continueTexture, NULL, &continueRect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(levelTexture);
	SDL_DestroyTexture(continueTexture);
	SDL_FreeSurface(levelSurface);
	SDL_FreeSurface(continueSurface);
	TTF_CloseFont(sixty);
	TTF_CloseFont(arial);
	SDL_DestroyWindow(menuWindow);
}
*/

void levelFinished(int level) {
// render the level finishing message
	SDL_Event e;
	int key_pressed = SDLK_a;

	SDL_Window *menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	WIDTH, HEIGHT, SDL_WINDOW_OPENGL); // | SDL_WINDOW_FULLSCREEN_DESKTOP
	SDL_Renderer * renderer = SDL_CreateRenderer(menuWindow, -1, 0);

	TTF_Font *sixty = TTF_OpenFont("data/misc/sixty.ttf", 50);
	TTF_Font *arial = TTF_OpenFont("data/misc/arial.ttf", 16);
	SDL_Color color = {255, 255, 255};

	std::string message = "Level " + std::to_string(level) + " finished.";

	int levelW = 0, levelH = 0;
	SDL_Surface *levelSurface = TTF_RenderText_Solid(sixty, message.c_str(), color);
	SDL_Texture *levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
	SDL_QueryTexture(levelTexture, NULL, NULL, &levelW, &levelH);
	SDL_Rect levelRect = {(WIDTH-levelW)/2, (HEIGHT-levelH)/2, levelW, levelH};

	int continueW = 0, continueH = 0;
	SDL_Surface *continueSurface = TTF_RenderText_Solid(arial, "Press Escape to continue.", color);
	SDL_Texture *continueTexture = SDL_CreateTextureFromSurface(renderer, continueSurface);
	SDL_QueryTexture(continueTexture, NULL, NULL, &continueW, &continueH);
	SDL_Rect continueRect = {(WIDTH-continueW)/2, (HEIGHT-levelH)/2+levelH+25, continueW, continueH};	

	while (key_pressed != SDLK_ESCAPE) {
		SDL_PollEvent(&e);
		if (e.type == SDL_KEYDOWN)
			 key_pressed = e.key.keysym.sym;
		SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
		SDL_RenderCopy(renderer, continueTexture, NULL, &continueRect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(levelTexture);
	SDL_DestroyTexture(continueTexture);
	SDL_FreeSurface(levelSurface);
	SDL_FreeSurface(continueSurface);
	TTF_CloseFont(sixty);
	TTF_CloseFont(arial);
	SDL_DestroyWindow(menuWindow);
}

void playerDied() {
	SDL_Event e;
	int key_pressed = SDLK_a;

	SDL_Window *menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	WIDTH, HEIGHT, SDL_WINDOW_OPENGL); // | SDL_WINDOW_FULLSCREEN_DESKTOP
	SDL_Renderer * renderer = SDL_CreateRenderer(menuWindow, -1, 0);

	TTF_Font *sixty = TTF_OpenFont("data/misc/sixty.ttf", 50);
	TTF_Font *arial = TTF_OpenFont("data/misc/arial.ttf", 16);
	SDL_Color color = {84, 84, 84};
	SDL_Color white = {255, 255, 255};
 	
	int levelW = 0, levelH = 0;
	SDL_Surface *levelSurface = TTF_RenderText_Solid(sixty, "You died. Noob.", color);
	SDL_Texture *levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
	SDL_QueryTexture(levelTexture, NULL, NULL, &levelW, &levelH);
	SDL_Rect levelRect = {(WIDTH-levelW)/2, (HEIGHT-levelH)/2, levelW, levelH};

	int continueW = 0, continueH = 0;
	SDL_Surface *continueSurface = TTF_RenderText_Solid(arial, "Press Escape to continue.", white);
	SDL_Texture *continueTexture = SDL_CreateTextureFromSurface(renderer, continueSurface);
	SDL_QueryTexture(continueTexture, NULL, NULL, &continueW, &continueH);
	SDL_Rect continueRect = {(WIDTH-continueW)/2, (HEIGHT-levelH)/2+levelH+25, continueW, continueH};	

	while (key_pressed != SDLK_ESCAPE) {
		SDL_PollEvent(&e);
		if (e.type == SDL_KEYDOWN)
			 key_pressed = e.key.keysym.sym;
		SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
		SDL_RenderCopy(renderer, continueTexture, NULL, &continueRect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(levelTexture);
	SDL_DestroyTexture(continueTexture);
	SDL_FreeSurface(levelSurface);
	SDL_FreeSurface(continueSurface);
	TTF_CloseFont(sixty);
	TTF_CloseFont(arial);
	SDL_DestroyWindow(menuWindow);
}

void resetPlayer() {
	hero.setHealth(hero.getBaseHealth());
	hero.resetPos();
	left = right = up = down = attack = false;
}

void genStars() {
	for (int i = 0; i < 100; i++) {
		starsArr[i].x = rand()%WIDTH;
		starsArr[i].y = rand()%HEIGHT;
	}
}

void updateStarPos(stars* s) {
	s->x += 2;
	if (s->x >= WIDTH)
		s->x = 0;
}

void draw() {
// will call the rendering functions
	while(running) {
		if (finished) {
			finished = false;
			play();
		}
		else if (playing)
			play();
			else
				menu();
	}

	TTF_Quit();
	SDL_Quit();
}