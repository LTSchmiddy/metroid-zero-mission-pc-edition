#pragma once
#include "../GameModHandler.h"
#include "SDL.h"
#include "SDL_version.h"

#include "../../gba/GBAinline.h"
#include "../../common/Types.h"
#include "../../System.h"
#include "../../Util.h"
#include "../../sdl/inputSDL.h"
#include "../../common/ConfigManager.h"
#include "../../common/Patch.h"

extern bool show_demo_window;
extern bool show_options;
extern bool show_quit;

void PauseMenu_Init(SDL_Window* window, SDL_GLContext* glcontext);

void PauseMenu_SetPause(bool setPause);
void PauseMenu_Toggle();
void PauseMenu_OnFocusLost();

void PauseMenu_handleSDLEvent(const SDL_Event& event);
void PauseMenu_handleInputSDLEvent(const SDL_Event& event);

void PauseMenu_MainLoop();

void PauseMenu_DrawDisplay(SDL_Texture* texture, SDL_Surface* surface, SDL_Window* window, SDL_Renderer* renderer);

void PauseMenu_OnEnd();