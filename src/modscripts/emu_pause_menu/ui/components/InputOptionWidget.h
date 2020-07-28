#pragma once
#include <string>
#include <vector>
#include "MenuOptionInfo.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

using namespace std;

class InputOptionWidget {
public:
	static std::vector<InputOptionWidget*> all;

	static bool AssignKeyToWaiting(unsigned keysym);

	
	string control_name = "";
	//string pref_name = "";

	UnsignedMenuOption* option;
	ImVec2* dimens = NULL;


	bool waiting_for_key = false;

	//InputOptionWidget();
	//InputOptionWidget(string p_control_name, string p_pref_name);
	InputOptionWidget(string p_control_name, UnsignedMenuOption* p_option);
	InputOptionWidget(string p_control_name, UnsignedMenuOption* p_option, ImVec2* p_dimens);
	
	~InputOptionWidget();

	void on_init();
	void on_destroy();

	void StartWaitingForKey();
	void SetKey(unsigned keysym);

	void Draw();
	void Draw(ImVec2* use_size);


};

