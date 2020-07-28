#include "../GameModHandler.h"
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_version.h"

#include "../../gba/GBAinline.h"
#include "../../common/Types.h"
#include "../../System.h"
#include "../../Util.h"
#include "../../sdl/inputSDL.h"
#include "../../common/ConfigManager.h"
#include "../../common/Patch.h"
#include "OverlayMenu_Main.h"
#include "ui/BaseComponent.h"
#include "ui/components/OptionsMenuComponent.h"
#include "ui/components/PauseMenuComponent.h"
#include "ui/components/QuitMenu.h"
#include "ui/components/InputOptionWidget.h"
#include "ui/components/MenuOptionInfo.h"
#include "Player.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "SDL.h"
#include "SDL_opengl.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include "modscripts/game_data_classes/MemAddrRef.h"

SDL_Rect menuRect;
SDL_Surface* menuSurface;
//SDL_Texture* menuTexture;


////SDL_Color fillColor;
//Uint32 fillColor = 0x00000000;
//Uint32 bgColor = 0xdd000000;

Uint32 rmask, gmask, bmask, amask;

bool show_demo_window = false;
//bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

ImGuiIO* io;

PauseMenu PM = PauseMenu();
OptionsMenu OM = OptionsMenu();
QuitMenu QM = QuitMenu();

ImGuiStyle* menuStyle;
ImFont* menuFont;

ImVec2 menuPadding = ImVec2(30, 30);

bool show_options;
bool show_quit;

//MemAddrRef<uint8_t> gamemode = MemAddrRef<uint8_t>(GamemodeAddress, MemAddrRefSize::SingleByte);

// Runtime Vars:
int lastXSize = -1;
int lastYSize = -1;
void PauseMenu_Init(SDL_Window* window, SDL_GLContext* gl_context) {


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->NavActive = true;

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsClassic();

	menuStyle = &ImGui::GetStyle();

	menuStyle->WindowPadding = menuPadding;
	menuStyle->WindowRounding = 0;
	menuStyle->WindowBorderSize = 0;
	menuStyle->ButtonTextAlign = ImVec2(0, 0.5);

	menuFont = io->Fonts->AddFontFromFileTTF("./assets/ui/SerpentineDBol.ttf", Mod_menuFontSize);
	io->Fonts->Build();
	io->FontDefault = menuFont;
	

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();
}

void PauseMenu_SetPause(bool setPause) {
    
	paused = setPause;
	if (paused) {
		soundPause();
		wasPaused = true;
	}

	else {
		soundResume();
		show_options = false;
	}



	PM.SetShow(paused);
	//Mod_Logln("Pause Toggle");
}


void PauseMenu_Toggle() {
	//if (!(CPUReadByte(GamemodeAddress) == 04 || CPUReadByte(GamemodeAddress) == 07)) {

	if (getGameMode() == GameMode::File_select) {
		show_options = !show_options;
		//PromptToQuit(false);
		return;

	}

	if (!(getGameMode() == GameMode::In_game || getGameMode() == GameMode::Chozodia_escape)) {
		return;
	}

	PauseMenu_SetPause(!paused);

	Mod_Logln("Pause Toggle");

}

void PauseMenu_OnFocusLost() {
	if (paused) {
		return;
	}

	if (!(getGameMode() == GameMode::In_game || getGameMode() == GameMode::Chozodia_escape)) {
		return;
	}

	PauseMenu_SetPause(true);

	Mod_Logln("Focus Lost");
}

// Modifying the key events passed to IMGUI, so that it recognizes the control inputs as valid menu navigation.
// There's probably a better way to do this. I'm just not about to look right now.

// imgui seems to use the arrow keys for nav and space for confirm...

//SDL_Event imguiSDLKeyEventMutator(const SDL_Event& event) {
//	
//	// Clones the event, since we don't want to alter the original:
//	SDL_Event retVal;
//	memcpy(&retVal, &event, sizeof(SDL_Event));
//
//	// Running Keycode Modifying 
//	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
//		// Allow using game controls:
//		if (event.key.keysym.sym == menu_options.input.up.value) { retVal.key.keysym.sym = SDLK_UP; }
//		else if (event.key.keysym.sym == menu_options.input.down.value) { retVal.key.keysym.sym = SDLK_DOWN; }
//		else if (event.key.keysym.sym == menu_options.input.left.value) { retVal.key.keysym.sym = SDLK_LEFT; }
//		else if (event.key.keysym.sym == menu_options.input.right.value) { retVal.key.keysym.sym = SDLK_RIGHT; }
//		else if (event.key.keysym.sym == menu_options.input.jump.value) { retVal.key.keysym.sym = SDLK_SPACE; }
//
//		// Allowing Enter key for confirm:
//		else if (event.key.keysym.sym == SDLK_RETURN) { 
//			retVal.key.keysym.sym = SDLK_SPACE; 
//			systemScreenMessage("Mutating ENTER!");
//		}
//	}
//
//	return retVal;
//}

void PauseMenu_handleSDLEvent(const SDL_Event& event) {

	//SDL_Event mutated_event = imguiSDLKeyEventMutator(event);
	//ImGui_ImplSDL2_ProcessEvent(&mutated_event);
	ImGui_ImplSDL2_ProcessEvent(&event);
}

//bool InputOptionWidget::AssignKeyToWaiting(unsigned keysym);
void PauseMenu_handleInputSDLEvent(const SDL_Event& event) {
	if (OM.GetShow()) {
		if (event.type == SDL_KEYDOWN) {
			InputOptionWidget::AssignKeyToWaiting(event.key.keysym.sym);
		}
	}
}

void PauseMenu_MainLoop() {

}

void PauseMenu_DrawDisplay(SDL_Texture* texture, SDL_Surface* surface, SDL_Window* window, SDL_Renderer* renderer) {

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();


	QM.SetShow(show_quit);
	QM.Draw(window);

	OM.SetShow(show_options);
	if (!show_quit) {
		OM.Draw(window);
	}


	//PM.ProcessWindow(window);
	if (!show_options && !show_quit) {
		PM.Draw(window);
	}


	if (paused) {
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}

	//		static float f = 0.0f;
	//		static int counter = 0;

	//		ImGui::Begin("Hello, world!", &paused);                          // Create a window called "Hello, world!" and append into it.

	//		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//		ImGui::Checkbox("Another Window", &show_another_window);

	//		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//			counter++;
	//		ImGui::SameLine();
	//		ImGui::Text("counter = %d", counter);

	//		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//		ImGui::End();

	//}
		// 3. Show another simple window.

	ImGui::Render();
	//glViewport(0, 0, (int)(io->DisplaySize.x), (int)(io->DisplaySize.y));
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	//ImGui_impl

	//SDL_GL_SwapWindow(window);

	//ImGui::Text(python_test().c_str());

}

void PauseMenu_OnEnd() {
	
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	delete io;
}