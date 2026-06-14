#pragma once
#include <SDL3/SDL.h>
extern SDL_AppResult APP_STATE;

typedef struct window_Info {
	const char* title;
	SDL_Point size;
	SDL_WindowFlags flag;
}window_Info;

extern window_Info WINDOW_INFO;
#define FPS 144.0
#define TIME (1.0f/FPS) //Seconds
#define SDL_CHECK(expression)\
	if(!expression){\
		APP_STATE = SDL_APP_FAILURE;\
		SDL_Log("SDL runtime error!\n%s: Line %i\n -> %s\n", __FILE__,__LINE__,SDL_GetError());\
	}

