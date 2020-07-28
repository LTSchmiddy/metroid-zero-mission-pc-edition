//#include "../src/modscripts/emu_pause_menu/ui/components/PauseMenuComponent.h"


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
#include "PauseMenuComponent.h"


//bool sdlLoadGameData();
bool ResetEmulation();

PauseMenu::PauseMenu() {
	name = "Pause:";
	windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
}

//void PauseMenu::ProcessWindow(SDL_Window* window) {
//	int winX, winY;
//	SDL_GetWindowSize(window, &winX, &winY);
//
//	ImGui::SetNextWindowSize(menuSize, 0);
//	ImGui::SetNextWindowPos(menuPos);
//
//	menuSize.x = winX;
//	menuSize.y = winY;
//}

void PauseMenu::Body() {
	ImGui::SetWindowFocus();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();


	ImGui::Text("Game Paused");

	ImGui::Separator();



	if (ImGui::Button("Resume")) {
		paused = false;
		SetShow(false);
	}

	ImGui::Spacing();
	//if (ImGui::Button("Python Test")) {
		//python_test();
	//}




	//if (show_demo_window && ImGui::Button("Hide Demo Window")) {
		//show_demo_window = false;
	//}
	//if (!show_demo_window && ImGui::Button("Show Demo Window")) {
		//show_demo_window = true;
	//}

	if (show_options && ImGui::Button("Hide Options")) {
		show_options = false;
	}
	if (!show_options && ImGui::Button("Show Options")) {
		show_options = true;
	}


	if (ImGui::Button("Quit to Main Menu")) {
		PromptToQuit(true);

	}


	if (ImGui::Button("Quit Game")) {
		//sdlLoadGameData();
		PromptToQuit(false);
	}

}