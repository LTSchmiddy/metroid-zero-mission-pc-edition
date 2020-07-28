#include "BaseComponent.h"
#include "../../GameModHandler.h"
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_version.h"

#include "../../gba/GBAinline.h"
#include "../../common/Types.h"
#include "../../System.h"
#include "../../Util.h"
#include "../../sdl/inputSDL.h"
#include "../../common/ConfigManager.h"
#include "../../common/Patch.h"
#include "../OverlayMenu_Main.h"


//~BaseMenu() {
//}

bool BaseMenu::GetShow() {
	return show;
}


void BaseMenu::SetShow(bool parShow) {
	if (show && !parShow) {
		justDisabled = true;
	}

	if (parShow && !show) {
		justEnabled = true;
	}

	show = parShow;
}

bool BaseMenu::ShowCheck() {
	return show;
}

void BaseMenu::Draw(SDL_Window* window) {
	//ShowCheck();

	BackgroundTasks();

	if (justEnabled) {
		OnEnable();
	}

	if (justDisabled) {
		OnDisable();
	}

	if (ShowCheck()) {
		ProcessWindow(window);
		DrawBegin();
		Body();
		DrawEnd();
	}

	justEnabled = false;
	justDisabled = false;
}

void BaseMenu::BackgroundTasks()
{
}

void BaseMenu::OnEnable()
{
}

void BaseMenu::OnDisable() {
}

void BaseMenu::ProcessWindow(SDL_Window* window) {
	SDL_GetWindowSize(window, &winX, &winY);


	window_aspect = (float)winX / (float)winY;
	game_aspect = (float)sizeX / (float)sizeY;



	if (window_aspect == game_aspect) { // A rarity, to be honest.
		menuSize.x = winX;
		menuPos.x = 0;

		menuSize.y = winY;
		menuPos.y = 0;

		//systemScreenMessage("Aspects are the same");
		
	}
	// Display is wider than game render:
	else if (window_aspect > game_aspect) {
		menuSize.x = winY * game_aspect;
		menuPos.x = (((float)winX) - menuSize.x) / 2;

		menuSize.y = winY;
		menuPos.y = 0;


	}
	// Display is taller than game render:
	else {
		menuSize.x = winX;
		menuPos.x = 0;

		menuSize.y = winX / game_aspect;
		menuPos.y = (((float)winY) - menuSize.y) / 2;

		//systemScreenMessage("Y is larger");
	}
	
	ImGui::SetNextWindowSize(menuSize, 0);
	ImGui::SetNextWindowPos(menuPos);
	
}

void BaseMenu::DrawBegin() {
	ImGui::Begin(name.c_str(), &show, windowFlags);
}

void BaseMenu::DrawEnd() {
	ImGui::End();
}

void BaseMenu::Body() {

}
