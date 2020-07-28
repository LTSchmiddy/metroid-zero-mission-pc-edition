#pragma once
//#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


//template <typename T>
class MenuOptionBase {
public:
	static vector<MenuOptionBase*> all_menu_options;

	
	static void save_all();
	static void load_all();


	string pref_name;
	//MenuOptionBase(string p_name, T p_value);
	MenuOptionBase();
	MenuOptionBase(string p_name);
	~MenuOptionBase();

	virtual void on_init();
	virtual void on_destroy();

	virtual void save();
	virtual void load();
};

//uint64_t

//class MenuOptionRef {
//public:
//	string name = "";
//	uint64_t ref = NULL;
//
//	MenuOptionRef(string p_name, uint64_t p_ref);
//	
//};

class BoolMenuOption : public MenuOptionBase {
public:
	BoolMenuOption(string p_name);
	~BoolMenuOption();

	bool value;

	void load();
	void save();
};

class IntMenuOption : public MenuOptionBase {
public:
	IntMenuOption(string p_name);
	~IntMenuOption();

	int value;

	void load();
	void save();
};

class UnsignedMenuOption : public MenuOptionBase {
public:
	UnsignedMenuOption(string p_name);
	~UnsignedMenuOption();

	unsigned value;

	void load();
	void save();
};

class StrMenuOption : public MenuOptionBase {
public:
	StrMenuOption(string p_name);
	~StrMenuOption();

	string value;

	void load();
	void save();
};

//extern vector<MenuOptionBase*> all_menu_options;



struct ShownOptions {
	struct ShownVideoOptions {
		BoolMenuOption fullscreen = BoolMenuOption("fullScreen");
		BoolMenuOption vsync = BoolMenuOption("vsync");
		UnsignedMenuOption filter = UnsignedMenuOption("filter");
		BoolMenuOption autoFrameSkip = BoolMenuOption("autoFrameSkip");

	} video;

	struct ShownAudioOptions {
		IntMenuOption volume = IntMenuOption("soundVolume");
		BoolMenuOption surround = BoolMenuOption("GBSurround");

	} audio;
	
	struct ShownInputOptions {
		UnsignedMenuOption up = UnsignedMenuOption("input:Up");
		UnsignedMenuOption down = UnsignedMenuOption("input:Down");
		UnsignedMenuOption left = UnsignedMenuOption("input:Left");
		UnsignedMenuOption right = UnsignedMenuOption("input:Right");
		UnsignedMenuOption jump = UnsignedMenuOption("input:Jump");
		UnsignedMenuOption fire = UnsignedMenuOption("input:Fire");
		UnsignedMenuOption quick_fire_missile = UnsignedMenuOption("input:QuickFireMissile");
		UnsignedMenuOption aim_up = UnsignedMenuOption("input:AimUp");
		UnsignedMenuOption aim_down = UnsignedMenuOption("input:AimDown");
		UnsignedMenuOption hold_for_missiles = UnsignedMenuOption("input:HoldForMissiles");
		UnsignedMenuOption map_and_equipment_menu = UnsignedMenuOption("input:MapAndEquipmentMenu");
		UnsignedMenuOption toggle_missile_type = UnsignedMenuOption("input:ToggleMissileType");
		UnsignedMenuOption quick_morphball = UnsignedMenuOption("input:QuickMorphball");
		UnsignedMenuOption fast_forward = UnsignedMenuOption("input:Joy0_Speed");
		UnsignedMenuOption capture = UnsignedMenuOption("input:Joy0_Capture");
	
	} input;

	struct ShownControlsOptions {
		BoolMenuOption use_2_button_aiming = BoolMenuOption("mods:force2ButtonAiming");
	} controls;

	struct ShownModOptions {
		BoolMenuOption disableHintStatues = BoolMenuOption("mods:disableHintStatues");
		BoolMenuOption betterEquipmentMenu = BoolMenuOption("mods:betterEquipmentMenu");
		BoolMenuOption healOnSave = BoolMenuOption("mods:healOnSave");
	} mods;

};

//extern ShownOptions menu_options;


struct sdl_name_ref {
	unsigned value;
	string hex;
	string name;
};

extern vector<unsigned> sdl_keynums;
extern vector<string> sdl_keynames;