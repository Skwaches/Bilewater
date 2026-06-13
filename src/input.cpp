#include "input.h"
#include "init.h"

void Inputs::newFrame(){
	pkeys = keys;
	pmouse = mouse;
}
		
void Inputs::loadEvent(const SDL_Event& event){
	switch(event.type){
		case SDL_EVENT_KEY_DOWN: 
			keys[event.key.scancode] = true;
			break;

		case SDL_EVENT_KEY_UP: 
			keys[event.key.scancode] = false;
			break;

		case SDL_EVENT_MOUSE_MOTION:
			mousePosition.x = event.motion.x;
			mousePosition.y = event.motion.y;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			mouse[event.button.button] = true;
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			mouse[event.button.button] = false;
			break;
		default:
			break;
	}
}

bool Inputs::isKeyHeld(SDL_Scancode key){
	return keys[key];
}
bool Inputs::isKeyClicked(SDL_Scancode key){
	return keys[key] && !pkeys[key];
}
bool Inputs::isKeyReleased(SDL_Scancode key){
	return !keys[key] && pkeys[key];
}
bool Inputs::mouseClicked(int button){
	return mouse[button] && !pmouse[button];
}
bool Inputs::mouseHeld(int button){
	return mouse[button]; 
}
bool Inputs::mouseReleased(int button){
	return !mouse[button] && pmouse[button];
}

