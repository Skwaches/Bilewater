#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "init.h"
#include "input.h"
#include "fluid.h"

SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

window_Info WINDOW_INFO = {"Bilewater", {0, 0}, SDL_WINDOW_FULLSCREEN};
SDL_AppResult APP_STATE = SDL_APP_CONTINUE;
Uint64 previous = 0;
Inputs inputs;
bool firstFrame = true;

//Test fluid
Fluid water(
	 {50,20}, //Dimensions
	  5,      //Radius
	 {30,50}, //Position
	 {30,40}, //Spacing
	 20, 	  //Accuracy
	{230.0/255, 51.0/255, 179.0/255, 1},//rgba(230,52,179,1)
	{30, 30}  //Grid size for collisions
);

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
				WINDOW_INFO.size.y, WINDOW_INFO.flag,
				&window, &renderer));

	previous = SDL_GetTicksNS();
	SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
	const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
	if(mode){
		WINDOW_INFO.size = {mode->w,mode->h};
	}
	SDL_Log("%i, %i", WINDOW_INFO.size.x, WINDOW_INFO.size.y);
	return APP_STATE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
	if(event->type == SDL_EVENT_QUIT){
		APP_STATE = SDL_APP_SUCCESS;
		return SDL_APP_SUCCESS;
	}
	else if(event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED){
		if(!firstFrame){
			//FIXME This outputs 1x1 if the window is resized by the SDL_WINDOW_FULLSCREEN flag.
		WINDOW_INFO.size = {event->window.data1, event->window.data2};
		SDL_Log("%i, %i", WINDOW_INFO.size.x, WINDOW_INFO.size.y);
	}
	}

	inputs.loadEvent(*event);
	return APP_STATE;
}

float unprocessedTime = 0;
const int SUBSTEPS = 3;
const float SUB_TIME = TIME/SUBSTEPS;
SDL_AppResult SDL_AppIterate(void *appstate){
		firstFrame = false;//FIXME This is a shitty work around
		inputs.newFrame();
		Uint64 current = SDL_GetTicksNS();
		float delay = (current - previous)/1000'000'000.0f;
	 	previous = current;

		if(delay > 0.25f) //Slow down the simulation if it's too slow. 
			delay = 0.25f;

		unprocessedTime += delay;
		while (unprocessedTime >= TIME){
			for(int i = 0; i < SUBSTEPS; i++){
				water.collisions(0.999f);
				water.update(SUB_TIME);
			}
			unprocessedTime -= TIME;
		}
		
		render(renderer);
		return APP_STATE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) //SDL handles cleanup for renderer and window
{ 
	
}
