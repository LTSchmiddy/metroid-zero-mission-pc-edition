#include "InputOptionWidget.h"


#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "SDL.h"
#include "SDLKeyLookup.h"

#include <vector>
#include <string>

using namespace std;
using namespace ImGui;



// Declaring the existence of the static member, for access.
vector<InputOptionWidget*> InputOptionWidget::all;

bool InputOptionWidget::AssignKeyToWaiting(unsigned keysym) {
	for (unsigned i = 0; i < all.size(); i++) {
		InputOptionWidget* checked = all.at(i);

		if (keysym == SDLK_ESCAPE) {
			checked->waiting_for_key = false;
		}
		else if (checked->waiting_for_key) {
			checked->SetKey(keysym);
			return true;
		}

	}
	return false;
}

InputOptionWidget::InputOptionWidget(string p_control_name, UnsignedMenuOption* p_option) {
	option = p_option;
	control_name = p_control_name;

	on_init();
}

InputOptionWidget::InputOptionWidget(string p_control_name, UnsignedMenuOption* p_option, ImVec2* p_dimens) {
	option = p_option;
	control_name = p_control_name;
	dimens = p_dimens;

	on_init();
}


InputOptionWidget::~InputOptionWidget() {
	delete dimens;
	on_destroy();
}

void InputOptionWidget::on_init() {
	all.push_back(this);

}


void InputOptionWidget::on_destroy() {
	for (unsigned i = 0; i < all.size(); i++) {
		if (all.at(i) != this) {
			continue;
		}

		//delete all_menu_options.at(i);
		all.erase(all.begin() + i);

		// I could break here, or I can leave it in case an option is entered twice.
		break;
	}
}

void InputOptionWidget::StartWaitingForKey() {
	for (unsigned i = 0; i < all.size(); i++) {
		InputOptionWidget* checked = all.at(i);

		if (checked == this) {
			continue;
		}
		
		checked->waiting_for_key = false;
	}

	waiting_for_key = true;

}

void InputOptionWidget::SetKey(unsigned keysym) {
	option->value = keysym;
	waiting_for_key = false;

}

void InputOptionWidget::Draw() {
	Draw(NULL);
}
void InputOptionWidget::Draw(ImVec2* use_size) {
	string display_text = control_name + ": ";

	if (waiting_for_key) {
		display_text += string("... waiting...");
	}
	else {
		display_text += sdl_key_lookup.GetDisplayName(option->value);
	}

	bool btn_return;
	
	if (use_size != NULL) {
		btn_return = Button(display_text.c_str(), *use_size);
	
	} else if (dimens != NULL) {
		btn_return = Button(display_text.c_str(), *dimens);
	
	} else {
		btn_return = Button(display_text.c_str());
	}

	if (btn_return) {
		StartWaitingForKey();
	}

}
