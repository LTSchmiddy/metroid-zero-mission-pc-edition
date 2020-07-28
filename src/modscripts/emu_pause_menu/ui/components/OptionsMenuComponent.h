//#pragma once
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
////#include "ui_assets/PauseMenu/PauseMenu_Header.h"
#include "../BaseComponent.h"
#include "MenuOptionInfo.h"
#include "InputOptionWidget.h"


//#include "imgui.h"
//#include "imgui_impl_sdl.h"
//#include "imgui_impl_opengl2.h"
//
//#include "SDL.h"
//#include "SDL_opengl.h"
//#include <GL/gl.h>
//#include <GL/glext.h>
//#include <GL/glu.h>
//
//#include <fstream>
#include <string>

using namespace std;

bool ResetEmulation();

extern ShownOptions menu_options;

struct InputOptionButtons {
	InputOptionWidget up = InputOptionWidget("Up", &menu_options.input.up);
	InputOptionWidget down = InputOptionWidget("Down", &menu_options.input.down);
	InputOptionWidget left = InputOptionWidget("Left", &menu_options.input.left);
	InputOptionWidget right = InputOptionWidget("Right", &menu_options.input.right);
	InputOptionWidget jump = InputOptionWidget("Jump", &menu_options.input.jump);
	InputOptionWidget fire = InputOptionWidget("Fire", &menu_options.input.fire);
	InputOptionWidget quick_fire_missile = InputOptionWidget("Quick Fire Missile", &menu_options.input.quick_fire_missile);
	InputOptionWidget aim_up = InputOptionWidget("Aim Up", &menu_options.input.aim_up);
	InputOptionWidget aim = InputOptionWidget("Aim", &menu_options.input.aim_up);
	InputOptionWidget aim_down = InputOptionWidget("Aim Down", &menu_options.input.aim_down);
	InputOptionWidget hold_for_missiles = InputOptionWidget("Hold For Missiles", &menu_options.input.hold_for_missiles);
	InputOptionWidget map_and_equipment_menu = InputOptionWidget("Map And Equipment Menu", &menu_options.input.map_and_equipment_menu);
	InputOptionWidget toggle_missile_type = InputOptionWidget("Toggle Missile Type", &menu_options.input.toggle_missile_type);
	InputOptionWidget quick_morphball = InputOptionWidget("Quick Morphball", &menu_options.input.quick_morphball);
	InputOptionWidget fast_forward = InputOptionWidget("Fast-Forward", &menu_options.input.fast_forward);
	InputOptionWidget capture = InputOptionWidget("Screen-Shot", &menu_options.input.capture);
};

extern InputOptionButtons input_option_buttons;

class OptionsMenu : public BaseMenu {
public:


	int selected = 0;


	std::string options_text;

	OptionsMenu();

	void OnEnable() override;

	void OnDisable() override;

	void ReadOptionsFile();
	void UpdateConfig();


	//void ProcessWindow(SDL_Window* window) override;

	void Body() override;

	void DrawVideoSettings();
	void DrawSoundSettings();
	void DrawInputSettings();
	void DrawModSettings();

	void OnSaveActions();
	void OnRevertActions();


	void ResetSoundVolume(bool do_load);
	void ResetSoundVolume();

};

