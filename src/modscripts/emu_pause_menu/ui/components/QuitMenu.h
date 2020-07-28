#include "modscripts/emu_pause_menu/ui/BaseComponent.h"

extern bool quit_to_menu;


void PromptToQuit(bool to_menu);

class QuitMenu : public BaseMenu {
public:
	//ImVec2 menuPos;
	//ImVec2 menuSize;

	int selected = 0;

	QuitMenu();


	void ProcessWindow(SDL_Window* window) override;

	void Body() override;

};