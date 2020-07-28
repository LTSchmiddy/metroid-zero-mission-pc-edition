#include "../BaseComponent.h"
#include "../../../GameModHandler.h"
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_version.h"

#include "sdl/inputSDL.h"

#include "../../gba/GBAinline.h"
#include "../../common/Types.h"
#include "../../System.h"
#include "../../Util.h"
#include "../../sdl/inputSDL.h"
#include "../../common/ConfigManager.h"
#include "../../common/Patch.h"
#include "../../OverlayMenu_Main.h"

#include "sdl/filters.h"

#include "QuitMenu.h"

#include "OptionsMenuComponent.h"
#include "MenuOptionInfo.h"
extern "C" {
#include "../common/iniparser.h"
}


using namespace ImGui;
using namespace std;

ShownOptions menu_options;
InputOptionButtons input_option_buttons;


void sdlSetFullScreen(bool);
void sdlSetFilter(int);

OptionsMenu::OptionsMenu() {
	name = "Options:";
	windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
}

void OptionsMenu::OnEnable() {
	//ReadOptionsFile();

	MenuOptionBase::load_all();
}

void OptionsMenu::OnDisable() {

	//systemScreenMessage("Disabling...");
	// Reset the sound volume:
	ResetSoundVolume();
}

void OptionsMenu::ReadOptionsFile() {
	ifstream fileIn("vbam.ini");

	if (fileIn.is_open()) {
		options_text.assign((std::istreambuf_iterator<char>(fileIn)),
			std::istreambuf_iterator<char>());


		Mod_Logln(options_text.c_str());

	}
}

void OptionsMenu::UpdateConfig() {

	LoadConfig();
	LoadInputConfig();
	Mod_LoadModSettings();
}

//void OptionsMenu::ProcessWindow(SDL_Window* window) {
	//int winX, winY;

//}

void OptionsMenu::Body() {
	ImGui::SetWindowFocus();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	if (Button("<-")) {
		show_options = false;
	}
	ImGui::SameLine();
	ImGui::Text("Options:");
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();

	if (ImGui::Button("Save")) {
		MenuOptionBase::save_all();
		SaveConfigFile();
		UpdateConfig();

		OnSaveActions();

	}

	ImGui::SameLine();
	if (ImGui::Button("Revert")) {
		MenuOptionBase::load_all();
		
		OnRevertActions();
	}

	if(getGameMode() == GameMode::File_select) {
		ImGui::SameLine();
		if (ImGui::Button("Quit Game")) {
			PromptToQuit(false);
		}

	}


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("OptionTabs", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Video")) {
			DrawVideoSettings();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Sound")) {
			DrawSoundSettings();
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem("Input")) {
			DrawInputSettings();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Gameplay")) {
			DrawModSettings();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::Separator();

	//ImGui::Text(string("window_aspect=" + to_string(window_aspect)).c_str());
	//ImGui::Text(string("game_aspect=" + to_string(game_aspect)).c_str());

}

void OptionsMenu::DrawVideoSettings() {

	Columns(2, "video_settings_columns");
	Text("Video:");
	Checkbox("Fullscreen", &menu_options.video.fullscreen.value);
	Checkbox("Vsync", &menu_options.video.vsync.value);
	Checkbox("Auto Frame Skip", &menu_options.video.autoFrameSkip.value);
	
	NextColumn();
	if (TreeNode("Upscaling Filter:")) {
		for (unsigned i = 0; i < NUM_OF_FILTERS; i++) {
			if (Selectable(getFilterName(i), menu_options.video.filter.value == i)) {
				menu_options.video.filter.value = i;

			}
		}
		TreePop();
	}
	Columns(1);
}

void OptionsMenu::DrawSoundSettings() {
	ImGui::SliderInt("Volume", &menu_options.audio.volume.value, 0, get_sound_max_volume() * 100);
	Checkbox("Use Surround Sound", &menu_options.audio.surround.value);

	ResetSoundVolume(false);


}

void OptionsMenu::DrawInputSettings() {
	Text("Input:");
	//ImVec2 input_set(winX / 3, 30);
	//ImVec2* input_size = &input_set;
	ImVec2* input_size = NULL;

	Columns(2, "input_settings_columns");
	Text("Keybinds:");
	//ImVec2* input_size = NULL;
	input_option_buttons.up.Draw(input_size);
	input_option_buttons.down.Draw(input_size);
	input_option_buttons.left.Draw(input_size);
	input_option_buttons.right.Draw(input_size);
	input_option_buttons.jump.Draw(input_size);
	input_option_buttons.fire.Draw(input_size);
	input_option_buttons.quick_fire_missile.Draw(input_size);

	if (menu_options.controls.use_2_button_aiming.value) {
		input_option_buttons.aim_up.Draw(input_size);
		input_option_buttons.aim_down.Draw(input_size);
	
	}
	else {
		input_option_buttons.aim.Draw(input_size);
	}
	
	
	input_option_buttons.hold_for_missiles.Draw(input_size);
	input_option_buttons.map_and_equipment_menu.Draw(input_size);
	input_option_buttons.toggle_missile_type.Draw(input_size);
	input_option_buttons.quick_morphball.Draw(input_size);

	Spacing();
	input_option_buttons.fast_forward.Draw(input_size);
	input_option_buttons.capture.Draw(input_size);

	NextColumn();
	Checkbox("2-Button Aiming", &menu_options.controls.use_2_button_aiming.value);

	//delete input_size;
	Columns(1);
}

void OptionsMenu::DrawModSettings() {
	Text("Gameplay Settings:");
	Columns(2, "input_settings_columns");

	Checkbox("Heal On Save", &menu_options.mods.healOnSave.value);
	
	NextColumn();
	Text("Requires Restarting the Game:");
	Checkbox("Disable Hint Statues", &menu_options.mods.disableHintStatues.value);
	Checkbox("Better Equipment Menu", &menu_options.mods.betterEquipmentMenu.value);

	Columns(1);
}


// If there's any settings that require special coding to update, handle that here:
void OptionsMenu::OnSaveActions() {
	should_update_fullscreen.set(menu_options.video.fullscreen.value);
	sdlSetFilter(menu_options.video.filter.value);
}


// If there's any settings that require special coding to revert (such as the volume), handle that here:
void OptionsMenu::OnRevertActions() {
	ResetSoundVolume(false);
}


// Used to make sure that the live-updating for the volume isn't used outside the menu.
void OptionsMenu::ResetSoundVolume(bool do_load) {
	if (do_load) {
		menu_options.audio.volume.load();
	}
	soundSetVolume(menu_options.audio.volume.value / 100.0);
}
void OptionsMenu::ResetSoundVolume() {
	ResetSoundVolume(true);
}