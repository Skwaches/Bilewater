#pragma once

#include <SDL3/SDL.h>
#include <string>
extern bool RUNNING;

#define SDL_CHECK(expression)\
	if(!expression){\
		RUNNING = false;\
		SDL_Log("SDL runtime error!\n%s: Line %i\n -> %s\n", __FILE__,__LINE__,SDL_GetError());\
	}
		
		
class Display{
	public:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Point size;
		Display(std::string title, int width, int height, SDL_WindowFlags flags);
		void destroy(void);
};

void init();
void render(Display& screen);
void quit(Display& screen);



