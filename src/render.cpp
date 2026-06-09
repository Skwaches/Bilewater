#include "init.h"

void render(Display &screen){
	SDL_CHECK(SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255));
	SDL_CHECK(SDL_RenderPresent(screen.renderer));
}
