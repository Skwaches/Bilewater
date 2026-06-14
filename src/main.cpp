#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "init.h"
#include "input.h"
#include "fluid.h"

SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
window_Info WINDOW_INFO = {"Bilewater", {1200, 1000}, };
SDL_AppResult APP_STATE = SDL_APP_CONTINUE;
Uint64 previous = 0;
Inputs inputs;

//Test fluid
Fluid water;

void render(SDL_Renderer* renderer){
	SDL_CHECK(SDL_SetRenderDrawColor(renderer,0,0,0,1));
	SDL_CHECK(SDL_RenderClear(renderer));
	water.draw(renderer);
	SDL_CHECK(SDL_RenderPresent(renderer));
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	SDL_SetAppMetadata("Bilewater", "1.0", "Fluid simulation");
SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
	SDL_CHECK(SDL_CreateWindowAndRenderer(
				WINDOW_INFO.title, WINDOW_INFO.size.x,
				WINDOW_INFO.size.y, WINDOW_INFO.flag, &window, &renderer));
	SDL_CHECK(SDL_SetRenderLogicalPresentation(renderer, WINDOW_INFO.size.x, WINDOW_INFO.size.y, SDL_LOGICAL_PRESENTATION_LETTERBOX));
	previous = SDL_GetTicksNS();
	return APP_STATE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
	if(event->type == SDL_EVENT_QUIT){
		APP_STATE = SDL_APP_SUCCESS;
		return SDL_APP_SUCCESS;
	}
	inputs.loadEvent(*event);
	return APP_STATE;
}

float unprocessedTime = 0;
SDL_AppResult SDL_AppIterate(void *appstate){
		inputs.newFrame();
		Uint64 current = SDL_GetTicksNS();
		float delay = (current - previous)/1000'000'000.0f;
	 	previous = current;

		if(delay > 0.25f)
			delay = 0.25f;

		unprocessedTime += delay;
		while (unprocessedTime >= TIME){
			water.collisions();
			water.update(TIME);
			unprocessedTime -= TIME;
		}
		render(renderer);
		return APP_STATE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) //SDL handles cleanup for renderer and window
{ 
	
}
