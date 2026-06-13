#pragma once
#include <SDL3/SDL.h>
extern SDL_AppResult APP_STATE;
#define SDL_CHECK(expression)\
	if(!expression){\
		APP_STATE = SDL_APP_FAILURE;\
		SDL_Log("SDL runtime error!\n%s: Line %i\n -> %s\n", __FILE__,__LINE__,SDL_GetError());\
	}

