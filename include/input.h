#pragma once
#include <SDL3/SDL.h>
#include <array>

class Inputs{
	 private:
		 std::array<bool, SDL_SCANCODE_COUNT> keys{}; 
		 std::array<bool, SDL_SCANCODE_COUNT> pkeys{}; //Previous keys state

		 std::array<bool, 6> mouse{};
		 std::array<bool, 6> pmouse{}; //Previous mouse state
		 SDL_Point mousePosition;

	 public:
		void newFrame();
		void loadEvent(const SDL_Event& event);

		bool isKeyClicked(SDL_Scancode key);
		bool isKeyHeld(SDL_Scancode key);
		bool isKeyReleased(SDL_Scancode key);

		bool mouseClicked(int button);
		bool mouseHeld(int button);
		bool mouseReleased(int button);

		SDL_Point cursor();
};

extern Inputs inputs;

