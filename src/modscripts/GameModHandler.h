//
// Created by Alex Schmid on 9/19/2019.
//
#pragma once




//#ifndef VISUALBOYADVANCE_M_GAMEMODHANDLER_H
//#define VISUALBOYADVANCE_M_GAMEMODHANDLER_H

#include "../gba/GBAinline.h"
#include "../sdl/inputSDL.h"
#include "sdl\filters.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <thread>

using namespace std;

//void GM_MainLoop();

//class GameModHandler {
//public:
//    
//};

//extern GameModHandler gameModHandler;
//static GameModHandler gameModHandler;

extern char * Mod_romPath;
extern char * Mod_menuPatchPath;
extern char * Mod_hintsPatchPath;

extern char * Mod_windowName;

extern bool Mod_betterEquipmentMenu;
extern bool Mod_noHints;
extern bool Mod_healOnSave;
extern int Mod_menuFontSize;

//extern bool Mod_maintainAspectRatio;

extern uint32_t GamemodeAddress;

struct should_do {
	bool should;
	bool val;

	void set(bool);
};

extern should_do should_update_fullscreen;
extern should_do should_video_resize;

enum GameMode {
	Soft_reset = 0x00,
	Intro = 0x01,
	Title = 0x02,
	File_select = 0x03,
	In_game = 0x04,
	Map_item_screen = 0x05,
	Game_over = 0x06,
	Chozodia_escape = 0x07,
	Credits = 0x08,
	Tourian_escape = 0x09,
	Cutscenes = 0x0A,
	Demo = 0x0B,
	Gallery = 0x0C,
	Metroid_Fusion_gallery = 0x0D,
	Erase_SRAM_menu = 0x0F,
	Debug_menu = 0x10,
};
GameMode getGameMode();

void Mod_InitLog();

void Mod_Init(SDL_Window* window, SDL_GLContext* glcontext);


void Mod_LoadModSettings();

void Mod_MainLoop();
//    void HandleEvent(wxEvent& event) {


void Mod_handleSDLEvent(const SDL_Event& event);

void Mod_handleInputSDLEvent(const SDL_Event& event);
bool Mod_HandleInput(int which, int button, bool pressed);
bool Mod_HandleInput(EPad which, EKey button, bool pressed);

void Mod_DrawDisplay(SDL_Texture* texture, SDL_Surface* surface, SDL_Window* window, SDL_Renderer* renderer);

void Mod_OnExit();

void Mod_Logln(const char * txt);
void Mod_Logln(char * txt);
void Mod_Logln(int num);


void Mod_Log(const char * txt);
void Mod_Log(char * txt);
void Mod_Log(int num);


struct tracked_key {
	unsigned key = 0;
	bool pressed = false;
};

struct tracked_keys {
	tracked_key quick_morphball;
};


//void Mod_Logln(char * str);
//#endif //VISUALBOYADVANCE_M_GAMEMODHANDLER_H
