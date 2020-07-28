#pragma once
//#include "../src/modscripts/GameModHandler.h"
//#include "SDL.h"
//#include "SDL_video.h"
//#include "SDL_version.h"
//
//#include "../../gba/GBAinline.h"
//#include "../../common/Types.h"
//#include "../../System.h"
//#include "../../Util.h"
//#include "../../sdl/inputSDL.h"
//#include "../../common/ConfigManager.h"
//#include "../../common/Patch.h"
//#include "../src/modscripts/emu_pause_menu/OverlayMenu_Main.h"
//#include "ui_assets/PauseMenu/PauseMenu_Header.h"
#include "modscripts/emu_pause_menu/ui/BaseComponent.h"


//#include "imgui.h"
//#include "imgui_impl_sdl.h"
//#include "imgui_impl_opengl2.h"

//#include "SDL.h"
//#include "SDL_opengl.h"
//#include <GL/gl.h>
//#include <GL/glext.h>
//#include <GL/glu.h>

//#include <fstream>
//#include <iostream>


using namespace std;

bool ResetEmulation();

class PauseMenu : public BaseMenu {
public:
	//ImVec2 menuPos;
	//ImVec2 menuSize;

	int selected = 0;

	PauseMenu();


	//void ProcessWindow(SDL_Window* window) override;

	void Body() override;

};

