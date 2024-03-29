#pragma once
#include <SDL.h>
#include <iostream>
#include <vector>

class CollisionComponent;

class Game
{
public:
	Game();
	~Game();

	void init(const char* title, int x, int y, int width, int height, bool fullscreen);
	void handleEvents();
	void update();
	void render();
	void close();
	static SDL_Renderer* renderer;
	
	bool running() { return isRunning; };
	static SDL_Event event;

	bool isRunning;
	
	enum groupLabels : std::size_t
	{
		groupMap,
		groupPlayers,
		groupColliders
	};
	
private:
	
	SDL_Window* window;

};
