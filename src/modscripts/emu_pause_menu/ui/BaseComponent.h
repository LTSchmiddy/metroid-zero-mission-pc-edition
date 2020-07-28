#pragma once
#include "../../GameModHandler.h"
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_version.h"

#include "gba/GBAinline.h"
#include "common/Types.h"
#include "System.h"
#include "Util.h"
#include "sdl/inputSDL.h"
#include "common/ConfigManager.h"
#include "common/Patch.h"
#include "../OverlayMenu_Main.h"
//#include "ui_assets/PauseMenu/PauseMenu_Header.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "SDL.h"
#include "SDL_opengl.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <string>
#include <fstream>
#include <iostream>

using namespace std;



class BaseMenu {
protected:
	bool show = false;
public: 
	string name;
	ImGuiWindowFlags windowFlags;

	ImVec2 menuPos;
	ImVec2 menuSize;
	int winX, winY;

	float window_aspect;
	float game_aspect;

	bool justEnabled = false;
	bool justDisabled = false;

	//BaseMenu() {
	//}

	//~BaseMenu() {
	//}
	virtual bool GetShow();

	virtual void SetShow(bool parShow);

	virtual bool ShowCheck();



	virtual void Draw(SDL_Window* window);

	virtual void BackgroundTasks();

	virtual void OnEnable();
	virtual void OnDisable();

	virtual void ProcessWindow(SDL_Window* window);

	virtual void DrawBegin();

	virtual void DrawEnd();

	virtual void Body();
};

