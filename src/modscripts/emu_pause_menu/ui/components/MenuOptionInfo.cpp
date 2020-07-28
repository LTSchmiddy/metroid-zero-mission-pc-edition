#include "MenuOptionInfo.h"
#include "common/ConfigManager.h"
#include <vector>
#include <string>


//vector<MenuOptionBase*> all_menu_options = vector<MenuOptionBase*>{};

vector<MenuOptionBase*> MenuOptionBase::all_menu_options;

// Base Option Methods:

void MenuOptionBase::save_all() {
	for (unsigned i = 0; i < all_menu_options.size(); i++) {
		all_menu_options.at(i)->save();
	}

}

void MenuOptionBase::load_all() {
	for (unsigned i = 0; i < all_menu_options.size(); i++) {
		all_menu_options.at(i)->load();
	}

}

MenuOptionBase::MenuOptionBase(){}

MenuOptionBase::MenuOptionBase(string p_name) {
	pref_name = p_name;
	on_init();
}

MenuOptionBase::~MenuOptionBase() {
	on_destroy();
}

void MenuOptionBase::on_init() {
	load();
	
	all_menu_options.push_back(this);
}

void MenuOptionBase::on_destroy() {
	for (unsigned i = 0; i < all_menu_options.size(); i++) {
		if (all_menu_options.at(i)->pref_name != this->pref_name) {
			continue;
		}

		//delete all_menu_options.at(i);
		all_menu_options.erase(all_menu_options.begin()+i);

		// I could break here, or I can leave it in case an option is entered twice.
	}
}

void MenuOptionBase::save() {}

void MenuOptionBase::load() {}



// Bool Option Methods =====================================:	
BoolMenuOption::BoolMenuOption(string p_name) {
	pref_name = p_name;
	on_init();
}
BoolMenuOption::~BoolMenuOption() {
	on_destroy();
}

void BoolMenuOption::load() {
	int rVal = ReadPref(pref_name.c_str());

	if (rVal == 0) {
		value = false;
	}
	else {
		value = true;
	}
}

void BoolMenuOption::save() {
	if (value == 0) {
		WritePref(pref_name.c_str(), 0);

	}
	else {
		WritePref(pref_name.c_str(), 1);
	}
	
}



// Int Option Methods =====================================:	
IntMenuOption::IntMenuOption(string p_name) {
	pref_name = p_name;
	on_init();
}
IntMenuOption::~IntMenuOption() {
	on_destroy();
}

void IntMenuOption::load() {
	value = ReadPref(pref_name.c_str());
}

void IntMenuOption::save() {
	WritePref(pref_name.c_str(), value);
}

// Unsigned Option Methods =====================================:	
UnsignedMenuOption::UnsignedMenuOption(string p_name) {
	pref_name = p_name;
	on_init();
}
UnsignedMenuOption::~UnsignedMenuOption() {
	on_destroy();
}

void UnsignedMenuOption::load() {
	value = ReadPref(pref_name.c_str());
}

void UnsignedMenuOption::save() {
	WritePref(pref_name.c_str(), value);
}



// String Option Methods =====================================:	
StrMenuOption::StrMenuOption(string p_name) {
	pref_name = p_name;
	on_init();
}
StrMenuOption::~StrMenuOption() {
	on_destroy();
}


void StrMenuOption::load() {
	value = string(ReadPrefString(pref_name.c_str()));
}

void StrMenuOption::save() {
	WritePrefString(pref_name.c_str(), value.c_str());
}
