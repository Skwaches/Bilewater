#include "init.h"
#include "input.h"


int main(){
	Display screen("Water", 400, 400, SDL_WINDOW_RESIZABLE);
	Inputs inputs;

	while(RUNNING){
		input(inputs);
		render(screen);
	}

	quit(screen);
	return 0;
}
