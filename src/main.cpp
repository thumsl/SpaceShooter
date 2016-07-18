#include <iostream>
#include "engine.h"

int main(int argc, char *args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	std::cout << "SDL started." << std::endl;

	init();
	draw();

	return 0;
}
