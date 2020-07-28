#include "../BaseComponent.h"
#include "../../../GameModHandler.h"
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
#include "../../OverlayMenu_Main.h"

#include "QuitMenu.h"

using namespace ImGui;

bool quit_to_menu = true;
extern int emulating;


void PromptToQuit(bool to_menu) {
	quit_to_menu = to_menu;
	show_quit = true;
}

bool ResetEmulation();
QuitMenu::QuitMenu() {
	name = "Quit?";
	windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

	menuSize = ImVec2(400, 200);
}

void QuitMenu::ProcessWindow(SDL_Window* window) {
	SDL_GetWindowSize(window, &winX, &winY);


	window_aspect = (float)winX / (float)winY;
	game_aspect = (float)sizeX / (float)sizeY;


	menuPos.x = (((float)winX) - menuSize.x) / 2;
	menuPos.y = (((float)winY) - menuSize.y) / 2;

	ImGui::SetNextWindowSize(menuSize, 0);
	ImGui::SetNextWindowPos(menuPos);

}

void QuitMenu::Body() {
	Text("Are you sure you want to quit?");
	Text("Any unsaved progress will be lost.");

	if (Button("Yes")) {
		if (quit_to_menu) {
			ResetEmulation();
			PauseMenu_SetPause(false);
			show_quit = false;
		}
		else {
			emulating = 0;
		}


	}

	if (Button("No")) {
		show_quit = false;
	}

}
