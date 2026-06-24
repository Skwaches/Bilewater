#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "fluid.h"
#include "init.h"
#include "input.h"

SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;

window_Info WINDOW_INFO = {.title = "Bilewater",
                           .rect = {.w = 500, .h = 300},
                           .flag = SDL_WINDOW_ALWAYS_ON_TOP};

SDL_AppResult APP_STATE = SDL_APP_CONTINUE;
Uint64 previous = 0;
Inputs inputs;
bool firstFrame = true;

// Test fluid
Fluid water({
    .viscosity = 1.0f,
    .density = 3.0f,
    .pressureMultiplier = 3.0f,
    .smoothingRadius = 20.0f,

    .radius = 3,

    .random = true,
    .range = {WINDOW_INFO.rect.w, WINDOW_INFO.rect.h},
    .position = {0.0f, 0.0f},
    .spacing = {10.0f, 10.0f},

    .dimensions = {40, 40},
    .gridSize = {60, 60},

    .accuracy = 4,
    .color = {0.900f, 0.400f, 0.400f, 1.0f},
});

void render(SDL_Renderer *renderer) {
  SDL_CHECK(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1));
  SDL_CHECK(SDL_RenderClear(renderer));
  water.draw(renderer);
  SDL_CHECK(SDL_RenderPresent(renderer));
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("Bilewater", "1.0", "Fluid simulation");

  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));

  SDL_CHECK(SDL_CreateWindowAndRenderer(WINDOW_INFO.title, WINDOW_INFO.rect.w,
                                        WINDOW_INFO.rect.h, WINDOW_INFO.flag,
                                        &window, &renderer));

  previous = SDL_GetTicksNS();
  if (WINDOW_INFO.flag & SDL_WINDOW_FULLSCREEN) {
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
    if (mode) {
      WINDOW_INFO.rect.w = mode->w;
      WINDOW_INFO.rect.h = mode->h;
    }
  }
  return APP_STATE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    APP_STATE = SDL_APP_SUCCESS;
    return SDL_APP_SUCCESS;
  } else if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
    if (!firstFrame) {
      // FIXME This outputs 1x1 if the window is resized by the
      // SDL_WINDOW_FULLSCREEN flag.
      WINDOW_INFO.rect.w = event->window.data1;
      WINDOW_INFO.rect.h = event->window.data2;
    }
  }

  inputs.loadEvent(*event);
  return APP_STATE;
}

float unprocessedTime = 0;
const int SUBSTEPS = 1;
const float SUB_TIME = TIME / SUBSTEPS;
SDL_AppResult SDL_AppIterate(void *appstate) {
  firstFrame = false; // FIXME This is a shitty work around
  inputs.newFrame();
  Uint64 current = SDL_GetTicksNS();
  float delay = (current - previous) / 1000'000'000.0f;
  previous = current;

  if (delay > 0.25f) // Slow down the simulation if it's too slow.
    delay = 0.25f;

  unprocessedTime += delay;
  while (unprocessedTime >= TIME) {
    for (int i = 0; i < SUBSTEPS; i++) {

      float force = 400, range = 100;
      if (inputs.mouseHeld(1))
        water.focus(force, range, inputs.cursor(), SUB_TIME);
      else if (inputs.mouseHeld(3))
        water.focus(-force, range, inputs.cursor(), SUB_TIME);

      water.update(SUB_TIME);
    }
    unprocessedTime -= TIME;
  }

  render(renderer);
  return APP_STATE;
}

void SDL_AppQuit(
    void *appstate,
    SDL_AppResult result) // SDL handles cleanup for renderer and window
{}
