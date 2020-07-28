//
// Created by Alex Schmid on 9/19/2019.
//



#include "GameModHandler.h"
#include "SDL.h"
#include "SDL_version.h"

#include "../gba/GBAinline.h"
#include "../common/Types.h"
#include "../System.h"
#include "../Util.h"
#include "../sdl/inputSDL.h"
#include "../common/ConfigManager.h"
#include "../common/Patch.h"
#include "modscripts/game_data_classes/MemAddrRef.h"
#include "Player.h"
#include "emu_pause_menu/OverlayMenu_Main.h"



#define GamemodeAddress 0x03000C70

//#include "../wx/wxvbam.h"
//GameModHandler gameModHandler = GameModHandler();
//
//void Mod_Logln(char * str) {
//	gameModHandler.Logln(str);
//}


//char * Mod_RomPath = "./Roms/main.gba";
//char * Mod_romPath = "./data/main/main.gba";
//char * Mod_menuPatchPath = "./data/main/equip_menu.ips";
//char * Mod_hintsPatchPath = "./data/main/disable_hints.ips";

char* Mod_romPath = "main.gba";
char* Mod_menuPatchPath = "equip_menu.ips";
char* Mod_hintsPatchPath = "disable_hints.ips";

char * Mod_windowName = "Metroid: Zero Mission - PC Edition";


// Mod Settings:
bool Mod_betterEquipmentMenu = false;
bool Mod_noHints = false;
bool Mod_force2ButtonAiming = false;
bool Mod_healOnSave = false;
//bool Mod_usePython = false;


int Mod_menuFontSize = 20;
bool Mod_maintainAspectRatio;


// Runtime Variables
ofstream logfile;

bool aimDownButtonHeld = false;
//int aimDownButtonStep = 0;
int loopTick = 0;
bool killThread = false;

bool upWasPressed = false;
bool downWasPressed = false;
bool isLPressed;

tracked_keys tkeys;

bool QuickMorphballLooper;


MemAddrRef<uint8_t> gamemode = MemAddrRef<uint8_t>(GamemodeAddress, MemAddrRefSize::SingleByte);

should_do should_update_fullscreen;
should_do should_video_resize;

//SamusPlayerData Samus;
struct SamusPlayerData {
	MemAddrRef<uint16_t> health = MemAddrRef<uint16_t>(SamusHealthBaseAddr, MemAddrRefSize::HalfWord);
	MemAddrRef<uint16_t> max_health = MemAddrRef<uint16_t>(SamusMaxHealthBaseAddr, MemAddrRefSize::HalfWord);

	MemAddrRef<uint8_t> pose = MemAddrRef<uint8_t>(SamusPoseAddr, MemAddrRefSize::SingleByte);
	MemAddrRef<uint8_t> cannon_direction = MemAddrRef<uint8_t>(SamusArmCannonDirAddr, MemAddrRefSize::SingleByte);
	MemAddrRef<uint8_t> diagonal_aiming_flag = MemAddrRef<uint8_t>(SamusDiagonalAimingFlagAddr, MemAddrRefSize::SingleByte);
} Samus;

SDL_version compiled;
SDL_version linked;

//SDL_Surface pauseSurface;
//SDL_Texture pauseTexture;

GameMode getGameMode() {
	return (GameMode)gamemode.get();
}

void Mod_InitLog() {
	logfile.open("runlog.txt");
	logfile << "Log File Start:\n";

}


void Mod_Init(SDL_Window* window, SDL_GLContext* glcontext) {

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	
	std::string lVerStr = "But we are linking against SDL version ";




	 //Display SDL Version Info:


	Mod_Log("We compiled against SDL version ");
	Mod_Log(compiled.major);
	Mod_Log(".");
	Mod_Log(compiled.minor);
	Mod_Log(".");
	Mod_Logln(compiled.patch);
		

	Mod_Log("But we are linking against SDL version ");
	Mod_Log(linked.major);
	Mod_Log(".");
	Mod_Log(linked.minor);
	Mod_Log(".");
	Mod_Logln(linked.patch);

	PauseMenu_Init(window, glcontext);
	

	//Mod.Logln
}

void Mod_LoadModSettings() {
	Mod_betterEquipmentMenu = ReadPref("mods:betterEquipmentMenu", 1);
	Mod_noHints = ReadPref("mods:disableHintStatues", 1);
	Mod_force2ButtonAiming = ReadPref("mods:force2ButtonAiming", 1);
	Mod_healOnSave = ReadPref("mods:healOnSave", 1);
	
	
	// Video/Display:
	//Mod_maintainAspectRatio = ReadPref("mvideo:maintainAspectRatio", 1);
	Mod_menuFontSize = ReadPref("mvideo:imguiFontSize", 20);

	//Inputs:
	tkeys.quick_morphball.key = ReadPref("input:QuickMorphball");

}

//void Mod_LoadPatches() {
//
//}

void Mod_MainLoop() {

	PauseMenu_MainLoop();

	//if (CPUReadByte(GamemodeAddress) != 04) {
	if (gamemode.get() != GameMode::In_game) {
		return;
	}

	
	// Heal on Save:
	if (Mod_healOnSave) {
		if (Samus.pose.get() == SamusPose::On_A_Save_Pad) {
			Samus.health.set(Samus.max_health.get());
			//Mod_Logln("Should be healing player...");
		}
	}
}

void Mod_handleSDLEvent(const SDL_Event& event) {
	PauseMenu_handleSDLEvent(event);
}

void Mod_handleInputSDLEvent(const SDL_Event& event) {
	PauseMenu_handleInputSDLEvent(event);

	if (event.key.keysym.sym == tkeys.quick_morphball.key) {
		if (event.type == SDL_KEYDOWN) {
			tkeys.quick_morphball.pressed = true;
		}

		if (event.type == SDL_KEYUP) {
			tkeys.quick_morphball.pressed = false;
		}
	}
}

bool Mod_HandleInput(int which, int button, bool pressed) {
	return Mod_HandleInput((EPad)which, (EKey)button, pressed);
}

bool Handle2ButtonAiming(EPad which, EKey button, bool pressed);
bool Mod_HandleInput(EPad which, EKey button, bool pressed) {
	if (which != EPad::PAD_1) {
		return pressed;
	}

	if (show_options) {
		return false;
	}

	//if (CPUReadByte(GamemodeAddress) != 04) {
	if (gamemode.get() != GameMode::In_game) {
		return pressed;
	}


	//if (inputReadJoypadButton(PAD_2, KEY_BUTTON_A)) {
	if (tkeys.quick_morphball.pressed && Samus.pose.get() != SamusPose::Morph_Ball) {
		if (button == KEY_DOWN) {
			QuickMorphballLooper = !QuickMorphballLooper;
			pressed = QuickMorphballLooper;
		}
		else if (button == KEY_UP || button == KEY_LEFT || button == KEY_RIGHT || button == KEY_BUTTON_L) {
			pressed = false;
		}
	}
	else {
		// Spoof for 2-Button Aiming:
		pressed = Handle2ButtonAiming(which, button, pressed);
	}

	// Quick-Fire Missiles:
	if ((button == KEY_BUTTON_B || button == KEY_BUTTON_R) && inputReadJoypadButton(PAD_2, KEY_BUTTON_B)) {
		pressed = true;
	}
	
	
	return pressed;

}

bool Handle2ButtonAiming(EPad which, EKey button, bool pressed) {
	aimDownButtonHeld = inputReadJoypadButton(PAD_2, KEY_BUTTON_L);

	if (Mod_force2ButtonAiming) {

		if (aimDownButtonHeld) {
			//if (button == EKey::KEY_DOWN) {
			//	if ((aimDownButtonStep >= 2 && aimDownButtonStep <= 5)){ //|| (aimDownButtonStep > 5 && CPUReadByte(0x03001414) != 0x02)) {
			//	
			//		pressed = true;
			//	
			//}

			//if (button == EKey::KEY_UP) {
			//	pressed = false;
			//}


			if (button == EKey::KEY_BUTTON_L) {
				pressed = true;
				//These Addresses control the player's aiming direction:
				Samus.diagonal_aiming_flag.set(SamusDiagonalAimingFlag::Down);
				Samus.cannon_direction.set(SamumArmCannonDirection::Diagonally_downwards);
				//aimDownButtonStep++;
			}


			if (Samus.pose.get() < SamusPose::Skidding) {
				if (button == KEY_UP && Samus.pose.get() != SamusPose::Crouching && Samus.pose.get() != SamusPose::Turning_Around_And_Crouching && Samus.pose.get() != SamusPose::Shooting_And_Crouching) {
					pressed = false;
				}

				// Handle standing up while aiming:
				else if (button == KEY_UP && pressed && !upWasPressed) {
					if (Samus.pose.get() == SamusPose::Crouching) {
						Samus.pose.set(SamusPose::Standing);
					}
					else if (Samus.pose.get() == SamusPose::Turning_Around_And_Crouching) {
						Samus.pose.set(SamusPose::Turning_Around);
					}
					else if (Samus.pose.get() == SamusPose::Shooting_And_Crouching) {
						Samus.pose.set(SamusPose::Shooting);
					}

					upWasPressed = pressed;
					pressed == false;
				}
				else {
					upWasPressed = pressed;
				}
			}
			else {
				if (button == KEY_UP) {
					upWasPressed = pressed;
				}

			}

		}
		else {

			if (button == KEY_BUTTON_L) {
				//if (Samus.pose.get() < 7) {
				//	if (inputReadJoypadButton(PAD_1, KEY_DOWN) && (Samus.pose.get() == 4 || Samus.pose.get() == 5 || Samus.pose.get() == 6)) {
				//		Samus.pose.set(11);
				//		pressed = false;
				//	}
				//}

				if (pressed) {
					//These Addresses control the player's aiming direction:
					Samus.diagonal_aiming_flag.set(SamusDiagonalAimingFlag::Up);
					Samus.cannon_direction.set(SamumArmCannonDirection::Diagonally_upwards);
				}

				isLPressed = pressed;
			}

			if (isLPressed) {
				if (Samus.pose.get() < SamusPose::Skidding) {
					//if (button == KEY_DOWN && Samus.pose.get() != 0 && Samus.pose.get() != 1 && Samus.pose.get() != 2 && Samus.pose.get() != 3) {
					//	pressed = false;
					//}

					// Handle standing up while aiming:
					if (button == KEY_DOWN && pressed && !downWasPressed) {
						//if (Samus.pose.get() == 1 || Samus.pose.get() == 4) {
						if (Samus.pose.get() == SamusPose::Standing || Samus.pose.get() == SamusPose::Crouching) {
							//Samus.pose.set(0x04);
							Samus.pose.set(SamusPose::Crouching);
							downWasPressed = pressed;
							pressed == false;
							return pressed;
						}
						//else if (Samus.pose.get() == 3) {
						else if (Samus.pose.get() == SamusPose::Shooting) {
							//Samus.pose.set(0x06);
							Samus.pose.set(SamusPose::Shooting_And_Crouching);
							downWasPressed = pressed;
							pressed == false;
							return pressed;
						}
						else if (Samus.pose.get() == SamusPose::Turning_Around) {
							//Samus.pose.set(0x05);
							Samus.pose.set(SamusPose::Turning_Around_And_Crouching);
							downWasPressed = pressed;
							pressed == false;
							return pressed;
						}

						downWasPressed = pressed;
						pressed == false;
					}
					else {
						downWasPressed = pressed;
					}
				}
				else {
					if (button == KEY_DOWN) {
						downWasPressed = pressed;
					}

				}
				Samus.diagonal_aiming_flag.set(SamusDiagonalAimingFlag::Up);
				Samus.cannon_direction.set(SamumArmCannonDirection::Diagonally_upwards);
			}
			else {
				if (button == KEY_DOWN) {
					downWasPressed = pressed;
				}

			}


			//aimDownButtonStep = 0;
		}
		//if (button == EKey::KEY_UP && inputReadJoypadButton(PAD_1, KEY_BUTTON_L) && CPUReadByte(0x03001414) != 0x01) {
		//	pressed = true;
		//}


	}
	else {
		if (aimDownButtonHeld) {

			if (button == EKey::KEY_BUTTON_L) {
				pressed = true;
			}
		}
	}

	return pressed;
}

void Mod_DrawDisplay(SDL_Texture* texture, SDL_Surface* surface, SDL_Window* window, SDL_Renderer* renderer) {
	PauseMenu_DrawDisplay(texture, surface, window, renderer);

}

void Mod_LogInputConfig() {
	for (int i = 0; i < 5; i++) {
		Mod_Log("Joypad: ");
		Mod_Logln(i);
		for (int j = 0; j < 14; j++) {
			Mod_Log("\tInput ");
			Mod_Log(j);
			Mod_Log(" = ");
			Mod_Logln(inputGetKeymap((EPad)i, (EKey)j));
		}
	}

	

}

void Mod_OnExit() {
	PauseMenu_OnEnd();
	logfile.flush();
	logfile.close();
}
// Logging:

void Mod_Logln(char * txt) {
	logfile << txt << "\n";
	//        printf(txt, 0);
}

void Mod_Logln(const char * txt) {
	logfile << txt << "\n";
	//        printf(txt, 0);
}

void Mod_Logln(int num) {
	logfile << num << "\n";
	//        printf(num, 0);
}

void Mod_Log(const char * txt) {
	logfile << txt;
	//        printf(txt, 0);
}

void Mod_Log(char * txt) {
	logfile << txt;
	//        printf(txt, 0);
}

void Mod_Log(int num) {
	logfile << num;
	//        printf(num, 0);
}

void should_do::set(bool nval) {
	val = nval;
	should = true;
}
