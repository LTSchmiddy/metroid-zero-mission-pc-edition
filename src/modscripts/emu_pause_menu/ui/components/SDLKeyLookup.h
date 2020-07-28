#pragma once
#include <vector>
#include <string>
#include <map>


using namespace std;

class SDLKeyLookup;

extern SDLKeyLookup sdl_key_lookup;

class SDLKeyLookup {
public:
	map<unsigned, string> key_name_table = {
		{0, "UNKNOWN"},
		{8, "BACKSPACE"},
		{9, "TAB"},
		{13, "RETURN"},
		{27, "ESCAPE"},
		{32, "SPACE"},
		{33, "EXCLAIM"},
		{34, "QUOTEDBL"},
		{35, "HASH"},
		{36, "DOLLAR"},
		{37, "PERCENT"},
		{38, "AMPERSAND"},
		{39, "QUOTE"},
		{40, "LEFTPAREN"},
		{41, "RIGHTPAREN"},
		{42, "ASTERISK"},
		{43, "PLUS"},
		{44, "COMMA"},
		{45, "MINUS"},
		{46, "PERIOD"},
		{47, "SLASH"},
		{48, "0"},
		{49, "1"},
		{50, "2"},
		{51, "3"},
		{52, "4"},
		{53, "5"},
		{54, "6"},
		{55, "7"},
		{56, "8"},
		{57, "9"},
		{58, "COLON"},
		{59, "SEMICOLON"},
		{60, "LESS"},
		{61, "EQUALS"},
		{62, "GREATER"},
		{63, "QUESTION"},
		{64, "AT"},
		{91, "LEFTBRACKET"},
		{92, "BACKSLASH"},
		{93, "RIGHTBRACKET"},
		{94, "CARET"},
		{95, "UNDERSCORE"},
		{96, "BACKQUOTE"},
		{97, "a"},
		{98, "b"},
		{99, "c"},
		{100, "d"},
		{101, "e"},
		{102, "f"},
		{103, "g"},
		{104, "h"},
		{105, "i"},
		{106, "j"},
		{107, "k"},
		{108, "l"},
		{109, "m"},
		{110, "n"},
		{111, "o"},
		{112, "p"},
		{113, "q"},
		{114, "r"},
		{115, "s"},
		{116, "t"},
		{117, "u"},
		{118, "v"},
		{119, "w"},
		{120, "x"},
		{121, "y"},
		{122, "z"},
		{127, "DELETE"},
		{1073741881, "CAPSLOCK"},
		{1073741882, "F1"},
		{1073741883, "F2"},
		{1073741884, "F3"},
		{1073741885, "F4"},
		{1073741886, "F5"},
		{1073741887, "F6"},
		{1073741888, "F7"},
		{1073741889, "F8"},
		{1073741890, "F9"},
		{1073741891, "F10"},
		{1073741892, "F11"},
		{1073741893, "F12"},
		{1073741894, "PRINTSCREEN"},
		{1073741895, "SCROLLLOCK"},
		{1073741896, "PAUSE"},
		{1073741897, "INSERT"},
		{1073741898, "HOME"},
		{1073741899, "PAGEUP"},
		{1073741901, "END"},
		{1073741902, "PAGEDOWN"},
		{1073741903, "RIGHT"},
		{1073741904, "LEFT"},
		{1073741905, "DOWN"},
		{1073741906, "UP"},
		{1073741907, "NUMLOCKCLEAR"},
		{1073741908, "KP_DIVIDE"},
		{1073741909, "KP_MULTIPLY"},
		{1073741910, "KP_MINUS"},
		{1073741911, "KP_PLUS"},
		{1073741912, "KP_ENTER"},
		{1073741913, "KP_1"},
		{1073741914, "KP_2"},
		{1073741915, "KP_3"},
		{1073741916, "KP_4"},
		{1073741917, "KP_5"},
		{1073741918, "KP_6"},
		{1073741919, "KP_7"},
		{1073741920, "KP_8"},
		{1073741921, "KP_9"},
		{1073741922, "KP_0"},
		{1073741923, "KP_PERIOD"},
		{1073741925, "APPLICATION"},
		{1073741926, "POWER"},
		{1073741927, "KP_EQUALS"},
		{1073741928, "F13"},
		{1073741929, "F14"},
		{1073741930, "F15"},
		{1073741931, "F16"},
		{1073741932, "F17"},
		{1073741933, "F18"},
		{1073741934, "F19"},
		{1073741935, "F20"},
		{1073741936, "F21"},
		{1073741937, "F22"},
		{1073741938, "F23"},
		{1073741939, "F24"},
		{1073741940, "EXECUTE"},
		{1073741941, "HELP"},
		{1073741942, "MENU"},
		{1073741943, "SELECT"},
		{1073741944, "STOP"},
		{1073741945, "AGAIN"},
		{1073741946, "UNDO"},
		{1073741947, "CUT"},
		{1073741948, "COPY"},
		{1073741949, "PASTE"},
		{1073741950, "FIND"},
		{1073741951, "MUTE"},
		{1073741952, "VOLUMEUP"},
		{1073741953, "VOLUMEDOWN"},
		{1073741957, "KP_COMMA"},
		{1073741958, "KP_EQUALSAS400"},
		{1073741977, "ALTERASE"},
		{1073741978, "SYSREQ"},
		{1073741979, "CANCEL"},
		{1073741980, "CLEAR"},
		{1073741981, "PRIOR"},
		{1073741982, "RETURN2"},
		{1073741983, "SEPARATOR"},
		{1073741984, "OUT"},
		{1073741985, "OPER"},
		{1073741986, "CLEARAGAIN"},
		{1073741987, "CRSEL"},
		{1073741988, "EXSEL"},
		{1073742000, "KP_00"},
		{1073742001, "KP_000"},
		{1073742002, "THOUSANDSSEPARATOR"},
		{1073742003, "DECIMALSEPARATOR"},
		{1073742004, "CURRENCYUNIT"},
		{1073742005, "CURRENCYSUBUNIT"},
		{1073742006, "KP_LEFTPAREN"},
		{1073742007, "KP_RIGHTPAREN"},
		{1073742008, "KP_LEFTBRACE"},
		{1073742009, "KP_RIGHTBRACE"},
		{1073742010, "KP_TAB"},
		{1073742011, "KP_BACKSPACE"},
		{1073742012, "KP_A"},
		{1073742013, "KP_B"},
		{1073742014, "KP_C"},
		{1073742015, "KP_D"},
		{1073742016, "KP_E"},
		{1073742017, "KP_F"},
		{1073742018, "KP_XOR"},
		{1073742019, "KP_POWER"},
		{1073742020, "KP_PERCENT"},
		{1073742021, "KP_LESS"},
		{1073742022, "KP_GREATER"},
		{1073742023, "KP_AMPERSAND"},
		{1073742024, "KP_DBLAMPERSAND"},
		{1073742025, "KP_VERTICALBAR"},
		{1073742026, "KP_DBLVERTICALBAR"},
		{1073742027, "KP_COLON"},
		{1073742028, "KP_HASH"},
		{1073742029, "KP_SPACE"},
		{1073742030, "KP_AT"},
		{1073742031, "KP_EXCLAM"},
		{1073742032, "KP_MEMSTORE"},
		{1073742033, "KP_MEMRECALL"},
		{1073742034, "KP_MEMCLEAR"},
		{1073742035, "KP_MEMADD"},
		{1073742036, "KP_MEMSUBTRACT"},
		{1073742037, "KP_MEMMULTIPLY"},
		{1073742038, "KP_MEMDIVIDE"},
		{1073742039, "KP_PLUSMINUS"},
		{1073742040, "KP_CLEAR"},
		{1073742041, "KP_CLEARENTRY"},
		{1073742042, "KP_BINARY"},
		{1073742043, "KP_OCTAL"},
		{1073742044, "KP_DECIMAL"},
		{1073742045, "KP_HEXADECIMAL"},
		{1073742048, "LCTRL"},
		{1073742049, "LSHIFT"},
		{1073742050, "LALT"},
		{1073742051, "LGUI"},
		{1073742052, "RCTRL"},
		{1073742053, "RSHIFT"},
		{1073742054, "RALT"},
		{1073742055, "RGUI"},
		{1073742081, "MODE"},
		{1073742082, "AUDIONEXT"},
		{1073742083, "AUDIOPREV"},
		{1073742084, "AUDIOSTOP"},
		{1073742085, "AUDIOPLAY"},
		{1073742086, "AUDIOMUTE"},
		{1073742087, "MEDIASELECT"},
		{1073742088, "WWW"},
		{1073742089, "MAIL"},
		{1073742090, "CALCULATOR"},
		{1073742091, "COMPUTER"},
		{1073742092, "AC_SEARCH"},
		{1073742093, "AC_HOME"},
		{1073742094, "AC_BACK"},
		{1073742095, "AC_FORWARD"},
		{1073742096, "AC_STOP"},
		{1073742097, "AC_REFRESH"},
		{1073742098, "AC_BOOKMARKS"},
		{1073742099, "BRIGHTNESSDOWN"},
		{1073742100, "BRIGHTNESSUP"},
		{1073742101, "DISPLAYSWITCH"},
		{1073742102, "KBDILLUMTOGGLE"},
		{1073742103, "KBDILLUMDOWN"},
		{1073742104, "KBDILLUMUP"},
		{1073742105, "EJECT"},
		{1073742106, "SLEEP"}
		
	};

	string GetDisplayName(unsigned num) {
		map<unsigned, string>::iterator search_result = key_name_table.find(num);

		if (search_result != key_name_table.end()) {
			return search_result->second;
		}

		return "ERR: Key Not Found";
	}
};

//map<unsigned, string> key_name_table = {
//		{0, "SDLK_UNKNOWN"},
//		{8, "SDLK_BACKSPACE"},
//		{9, "SDLK_TAB"},
//		{13, "SDLK_RETURN"},
//		{27, "SDLK_ESCAPE"},
//		{32, "SDLK_SPACE"},
//		{33, "SDLK_EXCLAIM"},
//		{34, "SDLK_QUOTEDBL"},
//		{35, "SDLK_HASH"},
//		{36, "SDLK_DOLLAR"},
//		{37, "SDLK_PERCENT"},
//		{38, "SDLK_AMPERSAND"},
//		{39, "SDLK_QUOTE"},
//		{40, "SDLK_LEFTPAREN"},
//		{41, "SDLK_RIGHTPAREN"},
//		{42, "SDLK_ASTERISK"},
//		{43, "SDLK_PLUS"},
//		{44, "SDLK_COMMA"},
//		{45, "SDLK_MINUS"},
//		{46, "SDLK_PERIOD"},
//		{47, "SDLK_SLASH"},
//		{48, "SDLK_0"},
//		{49, "SDLK_1"},
//		{50, "SDLK_2"},
//		{51, "SDLK_3"},
//		{52, "SDLK_4"},
//		{53, "SDLK_5"},
//		{54, "SDLK_6"},
//		{55, "SDLK_7"},
//		{56, "SDLK_8"},
//		{57, "SDLK_9"},
//		{58, "SDLK_COLON"},
//		{59, "SDLK_SEMICOLON"},
//		{60, "SDLK_LESS"},
//		{61, "SDLK_EQUALS"},
//		{62, "SDLK_GREATER"},
//		{63, "SDLK_QUESTION"},
//		{64, "SDLK_AT"},
//		{91, "SDLK_LEFTBRACKET"},
//		{92, "SDLK_BACKSLASH"},
//		{93, "SDLK_RIGHTBRACKET"},
//		{94, "SDLK_CARET"},
//		{95, "SDLK_UNDERSCORE"},
//		{96, "SDLK_BACKQUOTE"},
//		{97, "SDLK_a"},
//		{98, "SDLK_b"},
//		{99, "SDLK_c"},
//		{100, "SDLK_d"},
//		{101, "SDLK_e"},
//		{102, "SDLK_f"},
//		{103, "SDLK_g"},
//		{104, "SDLK_h"},
//		{105, "SDLK_i"},
//		{106, "SDLK_j"},
//		{107, "SDLK_k"},
//		{108, "SDLK_l"},
//		{109, "SDLK_m"},
//		{110, "SDLK_n"},
//		{111, "SDLK_o"},
//		{112, "SDLK_p"},
//		{113, "SDLK_q"},
//		{114, "SDLK_r"},
//		{115, "SDLK_s"},
//		{116, "SDLK_t"},
//		{117, "SDLK_u"},
//		{118, "SDLK_v"},
//		{119, "SDLK_w"},
//		{120, "SDLK_x"},
//		{121, "SDLK_y"},
//		{122, "SDLK_z"},
//		{127, "SDLK_DELETE"},
//		{1073741881, "SDLK_CAPSLOCK"},
//		{1073741882, "SDLK_F1"},
//		{1073741883, "SDLK_F2"},
//		{1073741884, "SDLK_F3"},
//		{1073741885, "SDLK_F4"},
//		{1073741886, "SDLK_F5"},
//		{1073741887, "SDLK_F6"},
//		{1073741888, "SDLK_F7"},
//		{1073741889, "SDLK_F8"},
//		{1073741890, "SDLK_F9"},
//		{1073741891, "SDLK_F10"},
//		{1073741892, "SDLK_F11"},
//		{1073741893, "SDLK_F12"},
//		{1073741894, "SDLK_PRINTSCREEN"},
//		{1073741895, "SDLK_SCROLLLOCK"},
//		{1073741896, "SDLK_PAUSE"},
//		{1073741897, "SDLK_INSERT"},
//		{1073741898, "SDLK_HOME"},
//		{1073741899, "SDLK_PAGEUP"},
//		{1073741901, "SDLK_END"},
//		{1073741902, "SDLK_PAGEDOWN"},
//		{1073741903, "SDLK_RIGHT"},
//		{1073741904, "SDLK_LEFT"},
//		{1073741905, "SDLK_DOWN"},
//		{1073741906, "SDLK_UP"},
//		{1073741907, "SDLK_NUMLOCKCLEAR"},
//		{1073741908, "SDLK_KP_DIVIDE"},
//		{1073741909, "SDLK_KP_MULTIPLY"},
//		{1073741910, "SDLK_KP_MINUS"},
//		{1073741911, "SDLK_KP_PLUS"},
//		{1073741912, "SDLK_KP_ENTER"},
//		{1073741913, "SDLK_KP_1"},
//		{1073741914, "SDLK_KP_2"},
//		{1073741915, "SDLK_KP_3"},
//		{1073741916, "SDLK_KP_4"},
//		{1073741917, "SDLK_KP_5"},
//		{1073741918, "SDLK_KP_6"},
//		{1073741919, "SDLK_KP_7"},
//		{1073741920, "SDLK_KP_8"},
//		{1073741921, "SDLK_KP_9"},
//		{1073741922, "SDLK_KP_0"},
//		{1073741923, "SDLK_KP_PERIOD"},
//		{1073741925, "SDLK_APPLICATION"},
//		{1073741926, "SDLK_POWER"},
//		{1073741927, "SDLK_KP_EQUALS"},
//		{1073741928, "SDLK_F13"},
//		{1073741929, "SDLK_F14"},
//		{1073741930, "SDLK_F15"},
//		{1073741931, "SDLK_F16"},
//		{1073741932, "SDLK_F17"},
//		{1073741933, "SDLK_F18"},
//		{1073741934, "SDLK_F19"},
//		{1073741935, "SDLK_F20"},
//		{1073741936, "SDLK_F21"},
//		{1073741937, "SDLK_F22"},
//		{1073741938, "SDLK_F23"},
//		{1073741939, "SDLK_F24"},
//		{1073741940, "SDLK_EXECUTE"},
//		{1073741941, "SDLK_HELP"},
//		{1073741942, "SDLK_MENU"},
//		{1073741943, "SDLK_SELECT"},
//		{1073741944, "SDLK_STOP"},
//		{1073741945, "SDLK_AGAIN"},
//		{1073741946, "SDLK_UNDO"},
//		{1073741947, "SDLK_CUT"},
//		{1073741948, "SDLK_COPY"},
//		{1073741949, "SDLK_PASTE"},
//		{1073741950, "SDLK_FIND"},
//		{1073741951, "SDLK_MUTE"},
//		{1073741952, "SDLK_VOLUMEUP"},
//		{1073741953, "SDLK_VOLUMEDOWN"},
//		{1073741957, "SDLK_KP_COMMA"},
//		{1073741958, "SDLK_KP_EQUALSAS400"},
//		{1073741977, "SDLK_ALTERASE"},
//		{1073741978, "SDLK_SYSREQ"},
//		{1073741979, "SDLK_CANCEL"},
//		{1073741980, "SDLK_CLEAR"},
//		{1073741981, "SDLK_PRIOR"},
//		{1073741982, "SDLK_RETURN2"},
//		{1073741983, "SDLK_SEPARATOR"},
//		{1073741984, "SDLK_OUT"},
//		{1073741985, "SDLK_OPER"},
//		{1073741986, "SDLK_CLEARAGAIN"},
//		{1073741987, "SDLK_CRSEL"},
//		{1073741988, "SDLK_EXSEL"},
//		{1073742000, "SDLK_KP_00"},
//		{1073742001, "SDLK_KP_000"},
//		{1073742002, "SDLK_THOUSANDSSEPARATOR"},
//		{1073742003, "SDLK_DECIMALSEPARATOR"},
//		{1073742004, "SDLK_CURRENCYUNIT"},
//		{1073742005, "SDLK_CURRENCYSUBUNIT"},
//		{1073742006, "SDLK_KP_LEFTPAREN"},
//		{1073742007, "SDLK_KP_RIGHTPAREN"},
//		{1073742008, "SDLK_KP_LEFTBRACE"},
//		{1073742009, "SDLK_KP_RIGHTBRACE"},
//		{1073742010, "SDLK_KP_TAB"},
//		{1073742011, "SDLK_KP_BACKSPACE"},
//		{1073742012, "SDLK_KP_A"},
//		{1073742013, "SDLK_KP_B"},
//		{1073742014, "SDLK_KP_C"},
//		{1073742015, "SDLK_KP_D"},
//		{1073742016, "SDLK_KP_E"},
//		{1073742017, "SDLK_KP_F"},
//		{1073742018, "SDLK_KP_XOR"},
//		{1073742019, "SDLK_KP_POWER"},
//		{1073742020, "SDLK_KP_PERCENT"},
//		{1073742021, "SDLK_KP_LESS"},
//		{1073742022, "SDLK_KP_GREATER"},
//		{1073742023, "SDLK_KP_AMPERSAND"},
//		{1073742024, "SDLK_KP_DBLAMPERSAND"},
//		{1073742025, "SDLK_KP_VERTICALBAR"},
//		{1073742026, "SDLK_KP_DBLVERTICALBAR"},
//		{1073742027, "SDLK_KP_COLON"},
//		{1073742028, "SDLK_KP_HASH"},
//		{1073742029, "SDLK_KP_SPACE"},
//		{1073742030, "SDLK_KP_AT"},
//		{1073742031, "SDLK_KP_EXCLAM"},
//		{1073742032, "SDLK_KP_MEMSTORE"},
//		{1073742033, "SDLK_KP_MEMRECALL"},
//		{1073742034, "SDLK_KP_MEMCLEAR"},
//		{1073742035, "SDLK_KP_MEMADD"},
//		{1073742036, "SDLK_KP_MEMSUBTRACT"},
//		{1073742037, "SDLK_KP_MEMMULTIPLY"},
//		{1073742038, "SDLK_KP_MEMDIVIDE"},
//		{1073742039, "SDLK_KP_PLUSMINUS"},
//		{1073742040, "SDLK_KP_CLEAR"},
//		{1073742041, "SDLK_KP_CLEARENTRY"},
//		{1073742042, "SDLK_KP_BINARY"},
//		{1073742043, "SDLK_KP_OCTAL"},
//		{1073742044, "SDLK_KP_DECIMAL"},
//		{1073742045, "SDLK_KP_HEXADECIMAL"},
//		{1073742048, "SDLK_LCTRL"},
//		{1073742049, "SDLK_LSHIFT"},
//		{1073742050, "SDLK_LALT"},
//		{1073742051, "SDLK_LGUI"},
//		{1073742052, "SDLK_RCTRL"},
//		{1073742053, "SDLK_RSHIFT"},
//		{1073742054, "SDLK_RALT"},
//		{1073742055, "SDLK_RGUI"},
//		{1073742081, "SDLK_MODE"},
//		{1073742082, "SDLK_AUDIONEXT"},
//		{1073742083, "SDLK_AUDIOPREV"},
//		{1073742084, "SDLK_AUDIOSTOP"},
//		{1073742085, "SDLK_AUDIOPLAY"},
//		{1073742086, "SDLK_AUDIOMUTE"},
//		{1073742087, "SDLK_MEDIASELECT"},
//		{1073742088, "SDLK_WWW"},
//		{1073742089, "SDLK_MAIL"},
//		{1073742090, "SDLK_CALCULATOR"},
//		{1073742091, "SDLK_COMPUTER"},
//		{1073742092, "SDLK_AC_SEARCH"},
//		{1073742093, "SDLK_AC_HOME"},
//		{1073742094, "SDLK_AC_BACK"},
//		{1073742095, "SDLK_AC_FORWARD"},
//		{1073742096, "SDLK_AC_STOP"},
//		{1073742097, "SDLK_AC_REFRESH"},
//		{1073742098, "SDLK_AC_BOOKMARKS"},
//		{1073742099, "SDLK_BRIGHTNESSDOWN"},
//		{1073742100, "SDLK_BRIGHTNESSUP"},
//		{1073742101, "SDLK_DISPLAYSWITCH"},
//		{1073742102, "SDLK_KBDILLUMTOGGLE"},
//		{1073742103, "SDLK_KBDILLUMDOWN"},
//		{1073742104, "SDLK_KBDILLUMUP"},
//		{1073742105, "SDLK_EJECT"},
//		{1073742106, "SDLK_SLEEP"}