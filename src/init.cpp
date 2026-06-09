#include "init.h"
bool RUNNING = true;
Display::Display(const std::string title, int width, int height, SDL_WindowFlags flags):
	size({width, height})
{
	SDL_CHECK(SDL_CreateWindowAndRenderer(&title[0], width, height,flags, &window, &renderer));
}

void Display::destroy(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void init(){
	SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
}

void quit(Display& screen){
	screen.destroy();
	SDL_Quit();
}
	

