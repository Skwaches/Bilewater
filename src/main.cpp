#include <cstdlib>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <csignal>

#include "init.hpp"
#include "input.hpp"
#include "fluid.hpp"
#include "shader.hpp"

SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;

window_Info WINDOW_INFO = {
	.title = "Bilewater",
	.rect = {.w = 400, .h = 400},
	.flag = SDL_WINDOW_ALWAYS_ON_TOP
};

std::atomic<SDL_AppResult> APP_STATE = SDL_APP_CONTINUE;
void signalHandler(int signum){
	static int attempts = 1;
	if(attempts > 2){
		SDL_Log("\nMultiple signals received! Force closing...");
		std::exit(signum);
	}
	SDL_Log("\nInterrupt signal %i received: Exiting", signum);
	APP_STATE = SDL_APP_SUCCESS;
	attempts++;
}

bool firstFrame = true;

//Test fluid
Fluid water({
		.points = 300,
		.target_Density = 3.0f,
		.viscosity = 1.0f,
		.pressureMultiplier = 3.0f,
		.smoothingRadius = 20.0f,
		.radius = 4.0f,

		.random = true ,

		.spacing = {10.0f,10.0f},
		.gravity = {0, 3},
		.region = WINDOW_INFO.rect,
		});

void render(SDL_Renderer* renderer){
	SDL_CHECK(SDL_SetRenderDrawColor(renderer,0,0,0,1));
	SDL_CHECK(SDL_RenderClear(renderer));
	water.drawGPU(renderer);
	SDL_CHECK(SDL_RenderPresent(renderer));
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	std::signal(SIGINT, signalHandler);
	SDL_SetAppMetadata("Bilewater", "1.0", "Fluid simulation");

	SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));

	SDL_CHECK(SDL_CreateWindowAndRenderer(
				WINDOW_INFO.title, WINDOW_INFO.rect.w,
				WINDOW_INFO.rect.h, WINDOW_INFO.flag,
				&window, &renderer));

	if(WINDOW_INFO.flag & SDL_WINDOW_FULLSCREEN){
		SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
		const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
		if(mode){
			WINDOW_INFO.rect.w = mode->w;
			WINDOW_INFO.rect.h = mode->h;
		}
	}
	kernel_Init(water.positions.size(), water.mesh_Vertices);
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
		WINDOW_INFO.rect.w = event->window.data1;
		WINDOW_INFO.rect.h = event->window.data2;
	}
	}

	inputs.loadEvent(*event);
	return APP_STATE;
}

SDL_AppResult SDL_AppIterate(void *appstate){
		firstFrame = false;//FIXME This is a shitty work around
		render(renderer);
		return APP_STATE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) //SDL handles cleanup for renderer and window
{ 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	kernel_Close();
	SDL_Quit();
}
