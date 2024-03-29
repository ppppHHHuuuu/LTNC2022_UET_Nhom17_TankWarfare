#pragma once
#include "Game.h"
#include "Menu.h"

Game* game = NULL;

Menu* menu = NULL;
int main(int argc,  char* argv[]) {
	menu->init("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1248, 736);
	Menu::handleOptionsMedia();
	Menu::loadMenuMedia();
	Menu::loadSoundMedia();

	const int FPS = 60;
	const int frameDelay = 1000 / 60;
	Uint32 frameStart;
	int frameTime;

	game = new Game();

	game->init("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1248, 736, false);
	menu->handleMenuEvent();
	while (game->running()) {
		frameStart = SDL_GetTicks();
		game->handleEvents();
		game->update();
		game->render();
		frameTime = SDL_GetTicks() - frameStart;
		// delay to only 60 frame rates per second . Source: Lets make game
		if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	}

	game->close();
	return 0;
}