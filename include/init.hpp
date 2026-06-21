#pragma once
#include <SDL3/SDL.h>
#include <atomic>
extern std::atomic<SDL_AppResult> APP_STATE;

typedef struct window_Info {
	const char* title = "Maiden";
	SDL_FRect rect = {0, 0, 700, 400};
	SDL_WindowFlags flag = SDL_WINDOW_BORDERLESS;
}window_Info;

extern window_Info WINDOW_INFO;
#define FPS 60.0f
#define TIME (1.0f/FPS) //Seconds
#define SDL_CHECK(expression)\
	if(!expression){\
		APP_STATE = SDL_APP_FAILURE;\
		SDL_Log("SDL runtime error!\n%s: Line %i\n -> %s\n", __FILE__,__LINE__,SDL_GetError());\
	}

