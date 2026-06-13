#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "init.h"
#include "input.h"

typedef struct window_Info {
	const char* title;
	SDL_Point size;
	SDL_WindowFlags flag;
}window_Info;

SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
window_Info bile = {"Bilewater", {800, 800}, SDL_WINDOW_RESIZABLE};
SDL_AppResult APP_STATE = SDL_APP_CONTINUE;
Inputs inputs;

void render(SDL_Renderer* renderer){
	SDL_CHECK(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
	SDL_CHECK(SDL_RenderPresent(renderer));
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	SDL_SetAppMetadata("Bilewater", "1.0", "Fluid simulation");
	SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
	SDL_CHECK(SDL_CreateWindowAndRenderer(
				bile.title, bile.size.x,
				bile.size.y, bile.flag, &window, &renderer));
	SDL_CHECK(SDL_SetRenderLogicalPresentation(renderer, bile.size.x, bile.size.y, SDL_LOGICAL_PRESENTATION_LETTERBOX));
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


SDL_AppResult SDL_AppIterate(void *appstate){
		render(renderer);
		return APP_STATE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) //SDL handles cleanup for renderer and window
{ 

}
