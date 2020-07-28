// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005-2006 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//OpenGL library
#if (defined _MSC_VER)
#pragma comment(lib, "OpenGL32")
#include <windows.h>
#endif

#include <cmath>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include <time.h>

//#include "..//version.h"
#include "../msbuild-release/version.h"
#include "version.h"



#include "SDL.h"
//#include "SDL_version.h"
#include "icon.h"

#include "../Util.h"
#include "../common/ConfigManager.h"
#include "../common/Patch.h"
#include "../gb/gb.h"
#include "../gb/gbCheats.h"
#include "../gb/gbGlobals.h"
#include "../gb/gbSound.h"
#include "../gba/Cheats.h"
#include "../gba/Flash.h"
#include "../gba/GBA.h"
#include "../gba/RTC.h"
#include "../gba/Sound.h"
#include "../gba/agbprint.h"

#include "../common/SoundSDL.h"
#include "filters.h"
#include "inputSDL.h"
#include "text.h"

#include "../modscripts/GameModHandler.h"
#include "../modscripts/emu_pause_menu/OverlayMenu_Main.h"

// from: https://stackoverflow.com/questions/7608714/why-is-my-pointer-not-null-after-free
#define freeSafe(ptr) free(ptr); ptr = NULL;

#ifndef _WIN32
#include <unistd.h>
#define GETCWD getcwd
#else // _WIN32
#include <direct.h>
#include <io.h>
#define GETCWD _getcwd
#define snprintf sprintf
#define stat _stat
#define access _access
#ifndef W_OK
    #define W_OK 2
#endif
#define mkdir(X,Y) (_mkdir(X))
// from: https://www.linuxquestions.org/questions/programming-9/porting-to-win32-429334/
#ifndef S_ISDIR
    #define S_ISDIR(mode)  (((mode) & _S_IFMT) == _S_IFDIR)
#endif
#endif // _WIN32

#ifndef __GNUC__
#define HAVE_DECL_GETOPT 0
#define __STDC__ 1
#include "getopt.h"
#else // ! __GNUC__
#define HAVE_DECL_GETOPT 1
#include <getopt.h>
#endif // ! __GNUC__

#if WITH_LIRC
#include <lirc/lirc_client.h>
#include <sys/poll.h>
#endif

extern void remoteInit();
extern void remoteCleanUp();
extern void remoteStubMain();
extern void remoteStubSignal(int, int);
extern void remoteOutput(const char*, uint32_t);
extern void remoteSetProtocol(int);
extern void remoteSetPort(int);

struct EmulatedSystem emulator = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    false,
    0
};

struct GLfloatCoords {
    GLfloat x;
    GLfloat y;

    string string_rep() {
        return string("(" + to_string(x) + "," + to_string(y) + ")");
    }
};

struct GLfloatRect {
    GLfloatCoords top_left;
    GLfloatCoords top_right;
    GLfloatCoords bottom_left;
    GLfloatCoords bottom_right;

    string string_rep() {
        return string(
            "top_left=" + top_left.string_rep() +
            ",top_right = " + top_right.string_rep() +
            ",bottom_left=" + bottom_left.string_rep() +
            ",bottom_right=" + bottom_right.string_rep()
        );
    }

} glRect;


struct GLViewportParams {
    GLint x;
    GLint y;
    GLsizei w;
    GLsizei h;
    

    string string_rep() {
        return string(
            "x=" + to_string(x) +
            ",y=" + to_string(y) +
            ",w=" + to_string(w) +
            ",h=" + to_string(h)
        );
    }

    void set() {
        glViewport(x, y, w, h);
    }

} glViewportParams;

SDL_Surface* surface = NULL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_GLContext glcontext;

int window_size_x;
int window_size_y;

int systemSpeed = 0;
int systemRedShift = 0;
int systemBlueShift = 0;
int systemGreenShift = 0;
int systemColorDepth = 0;
int systemVerbose = 0;
int systemFrameSkip = 0;
int systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

int srcPitch = 0;
int destWidth = 0;
int destHeight = 0;
int desktopWidth = 0;
int desktopHeight = 0;

uint8_t* delta = NULL;
static const int delta_size = 322 * 242 * 4;

int filter_enlarge = 2;

int cartridgeType = 3;

int textureSize = 256;
GLuint screenTexture = 0;
uint8_t* filterPix = 0;

int emulating = 0;
int RGB_LOW_BITS_MASK = 0x821;
uint32_t systemColorMap32[0x10000];
uint16_t systemColorMap16[0x10000];
uint16_t systemGbPalette[24];

char filename[2048];

static int rewindSerial = 0;

static int sdlSaveKeysSwitch = 0;
// if 0, then SHIFT+F# saves, F# loads (old VBA, ...)
// if 1, then SHIFT+F# loads, F# saves (linux snes9x, ...)
// if 2, then F5 decreases slot number, F6 increases, F7 saves, F8 loads

static int saveSlotPosition = 0; // default is the slot from normal F1
// internal slot number for undoing the last load
#define SLOT_POS_LOAD_BACKUP 8
// internal slot number for undoing the last save
#define SLOT_POS_SAVE_BACKUP 9

static int sdlOpenglScale = 1;
// will scale window on init by this much
static int sdlSoundToggledOff = 0;

extern int autoFireMaxCount;

#define REWIND_NUM 8
#define REWIND_SIZE 400000

enum VIDEO_SIZE {
    VIDEO_1X,
    VIDEO_2X,
    VIDEO_3X,
    VIDEO_4X,
    VIDEO_5X,
    VIDEO_6X,
    VIDEO_320x240,
    VIDEO_640x480,
    VIDEO_800x600,
    VIDEO_1024x768,
    VIDEO_1280x1024,
    VIDEO_OTHER
};

#define _stricmp strcasecmp

uint32_t throttleLastTime = 0;

bool pauseNextFrame = false;
int sdlMirroringEnable = 1;

//static int ignore_first_resize_event = 0;

/* forward */
void systemConsoleMessage(const char*);

char* home;
char homeConfigDir[1024];
char homeDataDir[1024];

char* gameDataFile;

char screenMessageBuffer[21];
uint32_t screenMessageTime = 0;

#define SOUND_MAX_VOLUME 2.0
#define SOUND_ECHO 0.2
#define SOUND_STEREO 0.15

static void sdlChangeVolume(float d)
{
    float oldVolume = soundGetVolume();
    float newVolume = oldVolume + d;

    if (newVolume < 0.0)
        newVolume = 0.0;
    if (newVolume > SOUND_MAX_VOLUME)
        newVolume = SOUND_MAX_VOLUME;

    if (fabs(newVolume - oldVolume) > 0.001) {
        char tmp[32];
        sprintf(tmp, "Volume: %i%%", (int)(newVolume * 100.0 + 0.5));
        systemScreenMessage(tmp);
        soundSetVolume(newVolume);
    }
}

#if WITH_LIRC
//LIRC code
bool LIRCEnabled = false;
int LIRCfd = 0;
static struct lirc_config* LIRCConfigInfo;

void StartLirc(void)
{
    fprintf(stdout, "Trying to start LIRC: ");
    //init LIRC and Record output
    LIRCfd = lirc_init("vbam", 1);
    if (LIRCfd == -1) {
        //it failed
        fprintf(stdout, "Failed\n");
    } else {
        fprintf(stdout, "Success\n");
        //read the config file
        char LIRCConfigLoc[2048];
        sprintf(LIRCConfigLoc, "%s%c%s", homeConfigDir, FILE_SEP, "lircrc");
        fprintf(stdout, "LIRC Config file:");
        if (lirc_readconfig(LIRCConfigLoc, &LIRCConfigInfo, NULL) == 0) {
            //check vbam dir for lircrc
            fprintf(stdout, "Loaded (%s)\n", LIRCConfigLoc);
        } else if (lirc_readconfig(NULL, &LIRCConfigInfo, NULL) == 0) {
            //check default lircrc location
            fprintf(stdout, "Loaded\n");
        } else {
            //it all failed
            fprintf(stdout, "Failed\n");
            LIRCEnabled = false;
        }
        LIRCEnabled = true;
    }
}

void StopLirc(void)
{
    //did we actually get lirc working at the start
    if (LIRCEnabled) {
        //if so free the config and deinit lirc
        fprintf(stdout, "Shuting down LIRC\n");
        lirc_freeconfig(LIRCConfigInfo);
        lirc_deinit();
        //set lirc enabled to false
        LIRCEnabled = false;
    }
}
#endif

#ifdef __MSC__
#define stat _stat
#define S_IFDIR _S_IFDIR
#endif

bool sdlCheckDirectory(const char* dir)
{
	Mod_Log("Looking for directory: ");
	Mod_Logln(dir);

    struct stat buf;

    if (!dir || !dir[0])
        return false;

    if (stat(dir, &buf) == 0)
    {
	if (!(buf.st_mode & S_IFDIR))
	{
	    fprintf(stderr, "Error: %s is not a directory\n", dir);
	    Mod_Log("Error - Not a directory: ");
	    Mod_Logln(dir);
	    return false;
	}
	return true;
    }
    else
    {
	fprintf(stderr, "Error: %s does not exist\n", dir);

	Mod_Log("Error - Directory does not exist: ");
	Mod_Logln(dir);
	return false;
    }
}

char* sdlGetFilename(char* name)
{
    char path[1024] = ""; // avoid warning about uninitialised value
    char *filename = strrchr(name, FILE_SEP);
    if (filename)
        strncpy(path, filename + 1, strlen(filename));
    else
        sprintf(path, "%s", name);
    return strdup(path);
}

char* sdlGetFilePath(char* name)
{
    char path[1024] = ""; // avoid warning about uninitialised value
    char *filename = strrchr(name, FILE_SEP);
    if (filename)
        strncpy(path, name, strlen(name) - strlen(filename));
    else
        sprintf(path, "%c%c", '.', FILE_SEP);
    return strdup(path);
}

//char* sdlGetSaveFilePath(char* name)
//{
//    return "./saves/";
//}

FILE* sdlFindFile(const char* name)
{
    char buffer[4096];
    char path[2048];

#ifdef _WIN32
#define PATH_SEP ";"
#define FILE_SEP '\\'
#define EXE_NAME "Metroid Zero Mission - PC Edition.exe"
#else // ! _WIN32
#define PATH_SEP ":"
#define FILE_SEP '/'
#define EXE_NAME "vbam"
#endif // ! _WIN32

    fprintf(stdout, "Searching for file %s\n", name);

    if (GETCWD(buffer, 2048)) {
        fprintf(stdout, "Searching current directory: %s\n", buffer);
    }

    FILE* f = fopen(name, "r");
    if (f != NULL) {
        return f;
    }

    if (homeDataDir) {
        fprintf(stdout, "Searching home directory: %s\n", homeDataDir);
        sprintf(path, "%s%c%s", homeDataDir, FILE_SEP, name);
        f = fopen(path, "r");
        if (f != NULL)
            return f;
    }

#ifdef _WIN32
    char* home = getenv("USERPROFILE");
    if (home != NULL) {
        fprintf(stdout, "Searching user profile directory: %s\n", home);
        sprintf(path, "%s%c%s", home, FILE_SEP, name);
        f = fopen(path, "r");
        if (f != NULL)
            return f;
    }

    if (!strchr(home, '/') && !strchr(home, '\\')) {
        char* path = getenv("PATH");

        if (path != NULL) {
            fprintf(stdout, "Searching PATH\n");
            strncpy(buffer, path, 4096);
            buffer[4095] = 0;
            char* tok = strtok(buffer, PATH_SEP);

            while (tok) {
                sprintf(path, "%s%c%s", tok, FILE_SEP, EXE_NAME);
                f = fopen(path, "r");
                if (f != NULL) {
                    char path2[2048];
                    fclose(f);
                    sprintf(path2, "%s%c%s", tok, FILE_SEP, name);
                    f = fopen(path2, "r");
                    if (f != NULL) {
                        fprintf(stdout, "Found at %s\n", path2);
                        return f;
                    }
                }
                tok = strtok(NULL, PATH_SEP);
            }
        }
    } else {
        // executable is relative to some directory
        fprintf(stdout, "Searching executable directory\n");
        strcpy(buffer, home);
        char* p = strrchr(buffer, FILE_SEP);
        if (p) {
            *p = 0;
            sprintf(path, "%s%c%s", buffer, FILE_SEP, name);
            f = fopen(path, "r");
            if (f != NULL)
                return f;
        }
    }
#else // ! _WIN32
    fprintf(stdout, "Searching data directory: %s\n", PKGDATADIR);
    sprintf(path, "%s%c%s", PKGDATADIR, FILE_SEP, name);
    f = fopen(path, "r");
    if (f != NULL)
        return f;

    fprintf(stdout, "Searching system config directory: %s\n", SYSCONF_INSTALL_DIR);
    sprintf(path, "%s%c%s", SYSCONF_INSTALL_DIR, FILE_SEP, name);
    f = fopen(path, "r");
    if (f != NULL)
        return f;
#endif // ! _WIN32

    return NULL;
}



static void sdlOpenGLScaleWithAspect(int w, int h)
{
    //systemScreenMessage(string(to_string(w) + ", " + to_string(h)).c_str());

    float screenAspect = (float)sizeX / sizeY,
          windowAspect = (float)w / h;


    if (windowAspect == screenAspect)
        //glViewport(0, 0, w, h);
        glViewportParams = { 0, 0, w, h };
    else if (windowAspect < screenAspect) {
        int height = (int)(w / screenAspect);
        glViewportParams = { 0, (h - height) / 2, w, height };
    } else {
        int width = (int)(h * screenAspect);
        glViewportParams = { (w - width) / 2, 0, width, h };
    }

    //systemScreenMessage(glViewportParams.string_rep().c_str());
    glViewportParams.set();

    /*systemScreenMessage("ScaleWithAspect ran...");*/
}

static void sdlOpenGLVideoResize()
{
    if (glIsTexture(screenTexture))
        glDeleteTextures(1, &screenTexture);

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        openGL == 2 ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        openGL == 2 ? GL_LINEAR : GL_NEAREST);

    // Calculate texture size as a the smallest working power of two
    float n1 = log10((float)destWidth) / log10(2.0f);
    float n2 = log10((float)destHeight) / log10(2.0f);
    float n = (n1 > n2) ? n1 : n2;

    // round up
    if (((float)((int)n)) != n)
        n = ((float)((int)n)) + 1.0f;

    textureSize = (int)pow(2.0f, n);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glClear(GL_COLOR_BUFFER_BIT);

    //sdlOpenGLScaleWithAspect(destWidth, destHeight);

    //int window_x;
    //int window_y;

    //SDL_GetWindowSize(window, &window_x, &window_y);
    sdlOpenGLScaleWithAspect(window_size_x, window_size_y);
}

void sdlOpenGLInit(int w, int h)
{
    (void)w; // unused params
    (void)h; // unused params
#if 0
  float screenAspect = (float) sizeX / sizeY,
        windowAspect = (float) w / h;

  if(glIsTexture(screenTexture))
  glDeleteTextures(1, &screenTexture);
#endif
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

#if 0
  if(windowAspect == screenAspect)
    glViewport(0, 0, w, h);
  else if (windowAspect < screenAspect) {
    int height = (int)(w / screenAspect);
    glViewport(0, (h - height) / 2, w, height);
  } else {
    int width = (int)(h * screenAspect);
    glViewport((w - width) / 2, 0, width, h);
  }
#endif

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#if 0
  glGenTextures(1, &screenTexture);
  glBindTexture(GL_TEXTURE_2D, screenTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  openGL == 2 ? GL_LINEAR : GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  openGL == 2 ? GL_LINEAR : GL_NEAREST);

  // Calculate texture size as a the smallest working power of two
  float n1 = log10((float)destWidth ) / log10( 2.0f);
  float n2 = log10((float)destHeight ) / log10( 2.0f);
  float n = (n1 > n2)? n1 : n2;

    // round up
  if (((float)((int)n)) != n)
    n = ((float)((int)n)) + 1.0f;

  textureSize = (int)pow(2.0f, n);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#endif

    glClearColor(0.0, 0.0, 0.0, 1.0);

    sdlOpenGLVideoResize();
}

static void sdlApplyPerImagePreferences()
{
    FILE* f = sdlFindFile("vba-over.ini");
    if (!f) {
        fprintf(stdout, "vba-over.ini NOT FOUND (using emulator settings)\n");
        return;
    } else
        fprintf(stdout, "Reading vba-over.ini\n");

    char buffer[7];
    buffer[0] = '[';
    buffer[1] = rom[0xac];
    buffer[2] = rom[0xad];
    buffer[3] = rom[0xae];
    buffer[4] = rom[0xaf];
    buffer[5] = ']';
    buffer[6] = 0;

    char readBuffer[2048];

    bool found = false;

    while (1) {
        char* s = fgets(readBuffer, 2048, f);

        if (s == NULL)
            break;

        char* p = strchr(s, ';');

        if (p)
            *p = 0;

        char* token = strtok(s, " \t\n\r=");

        if (!token)
            continue;
        if (strlen(token) == 0)
            continue;

        if (!strcmp(token, buffer)) {
            found = true;
            break;
        }
    }

    if (found) {
        while (1) {
            char* s = fgets(readBuffer, 2048, f);

            if (s == NULL)
                break;

            char* p = strchr(s, ';');
            if (p)
                *p = 0;

            char* token = strtok(s, " \t\n\r=");
            if (!token)
                continue;
            if (strlen(token) == 0)
                continue;

            if (token[0] == '[') // starting another image settings
                break;
            char* value = strtok(NULL, "\t\n\r=");
            if (value == NULL)
                continue;

            if (!strcmp(token, "rtcEnabled"))
                rtcEnable(atoi(value) == 0 ? false : true);
            else if (!strcmp(token, "flashSize")) {
                int size = atoi(value);
                if (size == 0x10000 || size == 0x20000)
                    flashSetSize(size);
            } else if (!strcmp(token, "saveType")) {
                int save = atoi(value);
                if (save >= 0 && save <= 5)
                    cpuSaveType = save;
            } else if (!strcmp(token, "mirroringEnabled")) {
                mirroringEnable = (atoi(value) == 0 ? false : true);
            }
        }
    }
    fclose(f);
}

static int sdlCalculateShift(uint32_t mask)
{
    int m = 0;

    while (mask) {
        m++;
        mask >>= 1;
    }

    return m - 5;
}

/* returns filename of savestate num, in static buffer (not reentrant, no need to free,
 * but value won't survive much - so if you want to remember it, dup it)
 * You may use the buffer for something else though - until you call sdlStateName again
 */
static char* sdlStateName(int num)
{
    static char stateName[2048];
    char *gameDir = sdlGetFilePath(filename);
    char *gameFile = sdlGetFilename(filename);

    if (saveDir)
        sprintf(stateName, "%s%c%s%d.sgm", saveDir, FILE_SEP, gameFile, num + 1);
    else if (access(gameDir, W_OK) == 0)
        sprintf(stateName, "%s%c%s%d.sgm", gameDir, FILE_SEP, gameFile, num + 1);
    else
        sprintf(stateName, "%s%c%s%d.sgm", homeDataDir, FILE_SEP, gameFile, num + 1);

    freeSafe(gameDir);
    freeSafe(gameFile);
    return stateName;
}

void sdlWriteState(int num)
{
    char* stateName;

    stateName = sdlStateName(num);

    if (emulator.emuWriteState)
        emulator.emuWriteState(stateName);

    // now we reuse the stateName buffer - 2048 bytes fit in a lot
    if (num == SLOT_POS_LOAD_BACKUP) {
        sprintf(stateName, "Current state backed up to %d", num + 1);
        systemScreenMessage(stateName);
    } else if (num >= 0) {
        sprintf(stateName, "Wrote state %d", num + 1);
        systemScreenMessage(stateName);
    }

    systemDrawScreen();
}

void sdlReadState(int num)
{
    char* stateName;

    stateName = sdlStateName(num);
    if (emulator.emuReadState)
        emulator.emuReadState(stateName);

    if (num == SLOT_POS_LOAD_BACKUP) {
        sprintf(stateName, "Last load UNDONE");
    } else if (num == SLOT_POS_SAVE_BACKUP) {
        sprintf(stateName, "Last save UNDONE");
    } else {
        sprintf(stateName, "Loaded state %d", num + 1);
    }
    systemScreenMessage(stateName);

    systemDrawScreen();
}

/*
 * perform savestate exchange
 * - put the savestate in slot "to" to slot "backup" (unless backup == to)
 * - put the savestate in slot "from" to slot "to" (unless from == to)
 */
void sdlWriteBackupStateExchange(int from, int to, int backup)
{
    char* dmp;
    char* stateNameOrig = NULL;
    char* stateNameDest = NULL;
    char* stateNameBack = NULL;

    dmp = sdlStateName(from);
    stateNameOrig = (char*)realloc(stateNameOrig, strlen(dmp) + 1);
    strcpy(stateNameOrig, dmp);
    dmp = sdlStateName(to);
    stateNameDest = (char*)realloc(stateNameDest, strlen(dmp) + 1);
    strcpy(stateNameDest, dmp);
    dmp = sdlStateName(backup);
    stateNameBack = (char*)realloc(stateNameBack, strlen(dmp) + 1);
    strcpy(stateNameBack, dmp);

    /* on POSIX, rename would not do anything anyway for identical names, but let's check it ourselves anyway */
    if (to != backup) {
        if (-1 == rename(stateNameDest, stateNameBack)) {
            fprintf(stdout, "savestate backup: can't backup old state %s to %s", stateNameDest, stateNameBack);
            perror(": ");
        }
    }
    if (to != from) {
        if (-1 == rename(stateNameOrig, stateNameDest)) {
            fprintf(stdout, "savestate backup: can't move new state %s to %s", stateNameOrig, stateNameDest);
            perror(": ");
        }
    }

    systemConsoleMessage("Savestate store and backup committed"); // with timestamp and newline
    fprintf(stdout, "to slot %d, backup in %d, using temporary slot %d\n", to + 1, backup + 1, from + 1);

    free(stateNameOrig);
    free(stateNameDest);
    free(stateNameBack);
}

void sdlWriteBattery()
{
    char buffer[2048];
    char *gameDir = sdlGetFilePath(filename);
    char *gameFile = sdlGetFilename(filename);

    if (batteryDir)
        sprintf(buffer, "%s%c%s.sav", batteryDir, FILE_SEP, gameFile);
    else if (access(gameDir, W_OK) == 0)
        sprintf(buffer, "%s%c%s.sav", gameDir, FILE_SEP, gameFile);
    else
        sprintf(buffer, "%s%c%s.sav", homeDataDir, FILE_SEP, gameFile);

    bool result = emulator.emuWriteBattery(buffer);

    //systemScreenMessage(gameFile);
    if (result)
	    systemMessage(0, "Wrote battery '%s'", buffer);
    
    freeSafe(gameFile);
    freeSafe(gameDir);
}

void sdlReadBattery()
{
    char buffer[2048];
    //char *gameDir = sdlGetFilePath(filename);
    char *gameDir = sdlGetFilePath(filename);
    char *gameFile = sdlGetFilename(filename);

    if (batteryDir)
        sprintf(buffer, "%s%c%s.sav", batteryDir, FILE_SEP, gameFile);
    else if (access(gameDir, W_OK) == 0)
        sprintf(buffer, "%s%c%s.sav", gameDir, FILE_SEP, gameFile);
    else
        sprintf(buffer, "%s%c%s.sav", homeDataDir, FILE_SEP, gameFile);

    bool result = emulator.emuReadBattery(buffer);

    //systemScreenMessage(gameFile);
    if (result)
        systemMessage(0, "Loaded battery '%s'", buffer);

    freeSafe(gameFile);
    freeSafe(gameDir);
}

void sdlReadDesktopVideoMode()
{
    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(window), &dm);
    desktopWidth = dm.w;
    desktopHeight = dm.h;
}

void sdlSetFullScreen(bool enabled) {
    fullScreen = enabled;
    SDL_SetWindowFullscreen(window, fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    if (openGL) {
        if (fullScreen)
            sdlOpenGLScaleWithAspect(desktopWidth, desktopHeight);
        else
            sdlOpenGLScaleWithAspect(destWidth, destHeight);
    }
}

static void sdlResizeVideo()
{
    //Line added by ALex. Keeps track of current video size.
    SDL_GetWindowSize(window, &window_size_x, &window_size_y);

    filter_enlarge = getFilterEnlargeFactor(filter);

    destWidth = filter_enlarge * sizeX;
    destHeight = filter_enlarge * sizeY;

    if (openGL) {
        free(filterPix);
        filterPix = (uint8_t*)calloc(1, (systemColorDepth >> 3) * destWidth * destHeight);
        sdlOpenGLVideoResize();
    }

    if (surface)
        SDL_FreeSurface(surface);
    if (texture)
        SDL_DestroyTexture(texture);

    if (!openGL) {
        surface = SDL_CreateRGBSurface(0, destWidth, destHeight, 32,
            0x00FF0000, 0x0000FF00,
            0x000000FF, 0xFF000000);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            destWidth, destHeight);
    }

    if (!openGL && surface == NULL) {
        systemMessage(0, "Failed to set video mode");
        SDL_Quit();
        exit(-1);
    }
}

void sdlSetFilter(int set_filter, bool do_window_update) {
    filter = set_filter;
    filterFunction = initFilter(filter, systemColorDepth, sizeX);

    if (do_window_update) {
        //if (getFilterEnlargeFactor(filter) != filter_enlarge) {

            /*if (!fullScreen)
                SDL_SetWindowSize(window, destWidth, destHeight);*/
        //}
        should_video_resize.should = true;
    }
}
void sdlSetFilter(int set_filter) {
    sdlSetFilter(set_filter, true);
}


void sdlInitVideo()
{
    int flags;
    int screenWidth;
    int screenHeight;

    filter_enlarge = getFilterEnlargeFactor(filter);

	destWidth = filter_enlarge * sizeX; // *2;
	destHeight = filter_enlarge * sizeY; // *2;

	//fullScreen = true;
	//openGL = true;

	//gameModHandler.Logln(openGL);

    flags = fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    if (openGL) {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        flags |= SDL_WINDOW_OPENGL;
    }

	flags |= SDL_WINDOW_RESIZABLE;

    screenWidth = destWidth;
    screenHeight = destHeight;

    if (window)
        SDL_DestroyWindow(window);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    //window = SDL_CreateWindow(Mod_windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, flags);
    window = SDL_CreateWindow(Mod_windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, flags);
    //window = SDL_CreateWindow("VBA-M", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, flags);
    //window = SDL_CreateWindow("VBA-M", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    if (!openGL) {
        renderer = SDL_CreateRenderer(window, -1, 0);
    }

    if (window == NULL) {
        systemMessage(0, "Failed to set video mode");
        SDL_Quit();
        exit(-1);
    }

    uint32_t rmask, gmask, bmask;

#if 0
  if(openGL) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
      rmask = 0x000000FF;
      gmask = 0x0000FF00;
      bmask = 0x00FF0000;
#else
      rmask = 0xFF000000;
      gmask = 0x00FF0000;
      bmask = 0x0000FF00;
#endif
  } else {
      rmask = surface->format->Rmask;
      gmask = surface->format->Gmask;
      bmask = surface->format->Bmask;
  }
#endif

    if (openGL) {
        rmask = 0xFF000000;
        gmask = 0x00FF0000;
        bmask = 0x0000FF00;
    } else {
        rmask = 0x00FF0000;
        gmask = 0x0000FF00;
        bmask = 0x000000FF;
    }

    systemRedShift = sdlCalculateShift(rmask);
    systemGreenShift = sdlCalculateShift(gmask);
    systemBlueShift = sdlCalculateShift(bmask);

    //printf("systemRedShift %d, systemGreenShift %d, systemBlueShift %d\n",
    //         systemRedShift, systemGreenShift, systemBlueShift);
    //  originally 3, 11, 19 -> 27, 19, 11

    if (openGL) {
        // Align to BGRA instead of ABGR
        systemRedShift += 8;
        systemGreenShift += 8;
        systemBlueShift += 8;
    }

#if 0
  if (openGL) {
    systemColorDepth = 0;
    int i;
    glcontext = SDL_GL_CreateContext(window);
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &i);
    systemColorDepth += i;
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &i);
    systemColorDepth += i;
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &i);
    systemColorDepth += i;
    printf("color depth (without alpha) is %d\n", systemColorDepth);
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &i);
    systemColorDepth += i;
    printf("color depth is %d\n", systemColorDepth);
  }
  else
    systemColorDepth = 32;

  if(systemColorDepth == 16) {
    srcPitch = sizeX*2 + 4;
  } else {
    if(systemColorDepth == 32)
      srcPitch = sizeX*4 + 4;
    else
      srcPitch = sizeX*3;
  }

#endif

    systemColorDepth = 32;
    srcPitch = sizeX * 4 + 4;

    if (openGL) {
        glcontext = SDL_GL_CreateContext(window);
        sdlOpenGLInit(screenWidth, screenHeight);
    }

    sdlResizeVideo();

#if 0
  if(openGL) {
    int scaledWidth = screenWidth * sdlOpenglScale;
    int scaledHeight = screenHeight * sdlOpenglScale;

    free(filterPix);
    filterPix = (uint8_t *)calloc(1, (systemColorDepth >> 3) * destWidth * destHeight);
    sdlOpenGLInit(screenWidth, screenHeight);

    if (	(!fullScreen)
	&&	sdlOpenglScale	> 1
	&&	scaledWidth	< desktopWidth
	&&	scaledHeight	< desktopHeight
    ) {
        SDL_SetVideoMode(scaledWidth, scaledHeight, 0,
                       SDL_OPENGL | SDL_RESIZABLE |
                       (fullScreen ? SDL_FULLSCREEN : 0));
        sdlOpenGLInit(scaledWidth, scaledHeight);
	/* xKiv: it would seem that SDL_RESIZABLE causes the *previous* dimensions to be immediately
	 * reported back via the SDL_VIDEORESIZE event
	 */
	ignore_first_resize_event	= 1;
    }
  }
#endif
}
#if defined(KMOD_GUI)
#define KMOD_META KMOD_GUI
#endif

#define MOD_KEYS (KMOD_CTRL | KMOD_SHIFT | KMOD_ALT | KMOD_META)
#define MOD_NOCTRL (KMOD_SHIFT | KMOD_ALT | KMOD_META)
#define MOD_NOALT (KMOD_CTRL | KMOD_SHIFT | KMOD_META)
#define MOD_NOSHIFT (KMOD_CTRL | KMOD_ALT | KMOD_META)

/*
 * 04.02.2008 (xKiv): factored out from sdlPollEvents
 *
 */
void change_rewind(int howmuch)
{
    if (emulating && emulator.emuReadMemState && rewindMemory
        && rewindCount) {
        rewindPos = (rewindPos + rewindCount + howmuch) % rewindCount;
        emulator.emuReadMemState(
            &rewindMemory[REWIND_SIZE * rewindPos],
            REWIND_SIZE);
        rewindCounter = 0;
        {
            char rewindMsgBuffer[50];
            sprintf(rewindMsgBuffer, "Rewind to %1d [%d]", rewindPos + 1, rewindSerials[rewindPos]);
            rewindMsgBuffer[49] = 0;
            systemConsoleMessage(rewindMsgBuffer);
        }
    }
}

/*
 * handle the F* keys (for savestates)
 * given the slot number and state of the SHIFT modifier, save or restore
 * (in savemode 3, saveslot is stored in saveSlotPosition and num means:
 *  4 .. F5: decrease slot number (down to 0)
 *  5 .. F6: increase slot number (up to 7, because 8 and 9 are reserved for backups)
 *  6 .. F7: save state
 *  7 .. F8: load state
 *  (these *should* be configurable)
 *  other keys are ignored
 * )
 */
static void sdlHandleSavestateKey(int num, int shifted)
{
    int action = -1;
    // 0: load
    // 1: save
    int backuping = 1; // controls whether we are doing savestate backups

    if (sdlSaveKeysSwitch == 2) {
        // ignore "shifted"
        switch (num) {
        // nb.: saveSlotPosition is base 0, but to the user, we show base 1 indexes (F## numbers)!
        case 4:
            if (saveSlotPosition > 0) {
                saveSlotPosition--;
                fprintf(stdout, "Changed savestate slot to %d.\n", saveSlotPosition + 1);
            } else
                fprintf(stderr, "Can't decrease slotnumber below 1.\n");
            return; // handled
        case 5:
            if (saveSlotPosition < 7) {
                saveSlotPosition++;
                fprintf(stdout, "Changed savestate slot to %d.\n", saveSlotPosition + 1);
            } else
                fprintf(stderr, "Can't increase slotnumber above 8.\n");
            return; // handled
        case 6:
            action = 1; // save
            break;
        case 7:
            action = 0; // load
            break;
        default:
            // explicitly ignore
            return; // handled
        }
    }

    if (sdlSaveKeysSwitch == 0) /* "classic" VBA: shifted is save */
    {
        if (shifted)
            action = 1; // save
        else
            action = 0; // load
        saveSlotPosition = num;
    }
    if (sdlSaveKeysSwitch == 1) /* "xKiv" VBA: shifted is load */
    {
        if (!shifted)
            action = 1; // save
        else
            action = 0; // load
        saveSlotPosition = num;
    }

    if (action < 0 || action > 1) {
        fprintf(
            stderr,
            "sdlHandleSavestateKey(%d,%d), mode %d: unexpected action %d.\n",
            num,
            shifted,
            sdlSaveKeysSwitch,
            action);
    }

    if (action) { /* save */
        if (backuping) {
            sdlWriteState(-1); // save to a special slot
            sdlWriteBackupStateExchange(-1, saveSlotPosition, SLOT_POS_SAVE_BACKUP); // F10
        } else {
            sdlWriteState(saveSlotPosition);
        }
    } else { /* load */
        if (backuping) {
            /* first back up where we are now */
            sdlWriteState(SLOT_POS_LOAD_BACKUP); // F9
        }
        sdlReadState(saveSlotPosition);
    }

} // sdlHandleSavestateKey

void sdlPollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
		Mod_handleSDLEvent(event);
        switch (event.type) {
        case SDL_QUIT:
            emulating = 0; 
            break;
#if 0
    case SDL_VIDEORESIZE:
      if (ignore_first_resize_event)
      {
	      ignore_first_resize_event	= 0;
	      break;
      }
      if (openGL)
      {
        SDL_SetVideoMode(event.resize.w, event.resize.h, 0,
                       SDL_OPENGL | SDL_RESIZABLE |
					   (fullScreen ? SDL_FULLSCREEN : 0));
        sdlOpenGLInit(event.resize.w, event.resize.h);
      }
      break;
    case SDL_ACTIVEEVENT:
      if(pauseWhenInactive && (event.active.state & SDL_APPINPUTFOCUS)) {
        active = event.active.gain;
        if(active) {
          if(!paused) {
            if(emulating)
              soundResume();
          }
        } else {
          wasPaused = true;
          if(pauseWhenInactive) {
            if(emulating)
              soundPause();
          }

          memset(delta,255,delta_size);
        }
      }
      break;
#endif
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            //case SDL_WINDOWEVENT_FOCUS_GAINED:
            //    if (pauseWhenInactive)
            //        if (paused) {
            //            if (emulating) {
            //                paused = 0;
            //                soundResume();
            //            }
            //        }
            //    break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                if (pauseWhenInactive) {
                    //wasPaused = true;
                    //if (emulating) {
                    //    paused = 1;
                    //    soundPause();
                    //}

                    //memset(delta, 255, delta_size);


                    PauseMenu_OnFocusLost();
                    
                }
                break;
            case SDL_WINDOWEVENT_RESIZED:
                if (openGL)
                    //sdlOpenGLVideoResize();
                    sdlResizeVideo();
                    //sdlOpenGLScaleWithAspect(event.window.data1, event.window.data2);

                break;
            }
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            if (fullScreen) {
                SDL_ShowCursor(SDL_ENABLE);
                mouseCounter = 120;
            }
            break;
        case SDL_JOYHATMOTION:
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        case SDL_JOYAXISMOTION:
        case SDL_KEYDOWN:
            inputProcessSDLEvent(event);
			//Mod_Logln("key down");
            break;
        case SDL_KEYUP:
			inputProcessSDLEvent(event);
            break;
        }
    }
}

#if WITH_LIRC
void lircCheckInput(void)
{
    if (LIRCEnabled) {
        //setup a poll (poll.h)
        struct pollfd pollLIRC;
        //values fd is the pointer gotten from lircinit and events is what way
        pollLIRC.fd = LIRCfd;
        pollLIRC.events = POLLIN;
        //run the poll
        if (poll(&pollLIRC, 1, 0) > 0) {
            //poll retrieved something
            char* CodeLIRC;
            char* CmdLIRC;
            int ret; //dunno???
            if (lirc_nextcode(&CodeLIRC) == 0 && CodeLIRC != NULL) {
                //retrieve the commands
                while ((ret = lirc_code2char(LIRCConfigInfo, CodeLIRC, &CmdLIRC)) == 0 && CmdLIRC != NULL) {
                    //change the text to uppercase
                    char* CmdLIRC_Pointer = CmdLIRC;
                    while (*CmdLIRC_Pointer != '\0') {
                        *CmdLIRC_Pointer = toupper(*CmdLIRC_Pointer);
                        CmdLIRC_Pointer++;
                    }

                    if (strcmp(CmdLIRC, "QUIT") == 0) {
                        emulating = 0;
                    } else if (strcmp(CmdLIRC, "PAUSE") == 0) {
                        paused = !paused;
                        if (paused)
                            soundPause();
                        else
                            soundResume();
                        if (paused)
                            wasPaused = true;
                        systemConsoleMessage(paused ? "Pause on" : "Pause off");
                        systemScreenMessage(paused ? "Pause on" : "Pause off");
                    } else if (strcmp(CmdLIRC, "RESET") == 0) {
                        if (emulating) {
                            emulator.emuReset();
                            systemScreenMessage("Reset");
                        }
                    } else if (strcmp(CmdLIRC, "MUTE") == 0) {
                        if (sdlSoundToggledOff) { // was off
                            // restore saved state
                            soundSetEnable(sdlSoundToggledOff);
                            sdlSoundToggledOff = 0;
                            systemConsoleMessage("Sound toggled on");
                        } else { // was on
                            sdlSoundToggledOff = soundGetEnable();
                            soundSetEnable(0);
                            systemConsoleMessage("Sound toggled off");
                            if (!sdlSoundToggledOff) {
                                sdlSoundToggledOff = 0x3ff;
                            }
                        }
                    } else if (strcmp(CmdLIRC, "VOLUP") == 0) {
                        sdlChangeVolume(0.1);
                    } else if (strcmp(CmdLIRC, "VOLDOWN") == 0) {
                        sdlChangeVolume(-0.1);
                    } else if (strcmp(CmdLIRC, "LOADSTATE") == 0) {
                        sdlReadState(saveSlotPosition);
                    } else if (strcmp(CmdLIRC, "SAVESTATE") == 0) {
                        sdlWriteState(saveSlotPosition);
                    } else if (strcmp(CmdLIRC, "1") == 0) {
                        saveSlotPosition = 0;
                        systemScreenMessage("Selected State 1");
                    } else if (strcmp(CmdLIRC, "2") == 0) {
                        saveSlotPosition = 1;
                        systemScreenMessage("Selected State 2");
                    } else if (strcmp(CmdLIRC, "3") == 0) {
                        saveSlotPosition = 2;
                        systemScreenMessage("Selected State 3");
                    } else if (strcmp(CmdLIRC, "4") == 0) {
                        saveSlotPosition = 3;
                        systemScreenMessage("Selected State 4");
                    } else if (strcmp(CmdLIRC, "5") == 0) {
                        saveSlotPosition = 4;
                        systemScreenMessage("Selected State 5");
                    } else if (strcmp(CmdLIRC, "6") == 0) {
                        saveSlotPosition = 5;
                        systemScreenMessage("Selected State 6");
                    } else if (strcmp(CmdLIRC, "7") == 0) {
                        saveSlotPosition = 6;
                        systemScreenMessage("Selected State 7");
                    } else if (strcmp(CmdLIRC, "8") == 0) {
                        saveSlotPosition = 7;
                        systemScreenMessage("Selected State 8");
                    } else {
                        //do nothing
                    }
                }
                //we dont need this code nomore
                free(CodeLIRC);
            }
        }
    }
}
#endif

void usage(char* cmd)
{
    printf("%s [option ...] file\n", cmd);
    printf("\
\n\
Options:\n\
  -O, --opengl=MODE            Set OpenGL texture filter\n\
      --no-opengl               0 - Disable OpenGL\n\
      --opengl-nearest          1 - No filtering\n\
      --opengl-bilinear         2 - Bilinear filtering\n\
  -F, --fullscreen             Full screen\n\
  -G, --gdb=PROTOCOL           GNU Remote Stub mode:\n\
                                tcp      - use TCP at port 55555\n\
                                tcp:PORT - use TCP at port PORT\n\
                                pipe     - use pipe transport\n\
  -I, --ifb-filter=FILTER      Select interframe blending filter:\n\
");
    for (int i = 0; i < (int)kInvalidIFBFilter; i++)
        printf("                                %d - %s\n", i, getIFBFilterName((IFBFilter)i));
    printf("\
  -N, --no-debug               Don't parse debug information\n\
  -S, --flash-size=SIZE        Set the Flash size\n\
      --flash-64k               0 -  64K Flash\n\
      --flash-128k              1 - 128K Flash\n\
  -T, --throttle=THROTTLE      Set the desired throttle (5...1000)\n\
  -b, --bios=BIOS              Use given bios file\n\
  -c, --config=FILE            Read the given configuration file\n\
  -f, --filter=FILTER          Select filter:\n\
");
    for (int i = 0; i < (int)kInvalidFilter; i++)
        printf("                                %d - %s\n", i, getFilterName((Filter)i));
    printf("\
  -h, --help                   Print this help\n\
  -i, --patch=PATCH            Apply given patch\n\
  -p, --profile=[HERTZ]        Enable profiling\n\
  -s, --frameskip=FRAMESKIP    Set frame skip (0...9)\n\
  -t, --save-type=TYPE         Set the available save type\n\
      --save-auto               0 - Automatic (EEPROM, SRAM, FLASH)\n\
      --save-eeprom             1 - EEPROM\n\
      --save-sram               2 - SRAM\n\
      --save-flash              3 - FLASH\n\
      --save-sensor             4 - EEPROM+Sensor\n\
      --save-none               5 - NONE\n\
  -v, --verbose=VERBOSE        Set verbose logging (trace.log)\n\
                                  1 - SWI\n\
                                  2 - Unaligned memory access\n\
                                  4 - Illegal memory write\n\
                                  8 - Illegal memory read\n\
                                 16 - DMA 0\n\
                                 32 - DMA 1\n\
                                 64 - DMA 2\n\
                                128 - DMA 3\n\
                                256 - Undefined instruction\n\
                                512 - AGBPrint messages\n\
\n\
Long options only:\n\
      --agb-print              Enable AGBPrint support\n\
      --auto-frameskip         Enable auto frameskipping\n\
      --no-agb-print           Disable AGBPrint support\n\
      --no-auto-frameskip      Disable auto frameskipping\n\
      --no-patch               Do not automatically apply patch\n\
      --no-pause-when-inactive Don't pause when inactive\n\
      --no-rtc                 Disable RTC support\n\
      --no-show-speed          Don't show emulation speed\n\
      --no-throttle            Disable throttle\n\
      --pause-when-inactive    Pause when inactive\n\
      --rtc                    Enable RTC support\n\
      --show-speed-normal      Show emulation speed\n\
      --show-speed-detailed    Show detailed speed data\n\
      --cheat 'CHEAT'          Add a cheat\n\
");
}

/*
 * 04.02.2008 (xKiv) factored out, reformatted, more usefuler rewinds browsing scheme
 */
void handleRewinds()
{
    int curSavePos; // where we are saving today [1]

    rewindCount++; // how many rewinds will be stored after this store
    if (rewindCount > REWIND_NUM)
        rewindCount = REWIND_NUM;

    curSavePos = (rewindTopPos + 1) % rewindCount; // [1] depends on previous
    long resize;
    if (
        emulator.emuWriteMemState
        && emulator.emuWriteMemState(
               &rewindMemory[curSavePos * REWIND_SIZE],
               REWIND_SIZE, /* available*/
               resize /* actual size */
               )) {
        char rewMsgBuf[100];
        sprintf(rewMsgBuf, "Remembered rewind %1d (of %1d), serial %d.", curSavePos + 1, rewindCount, rewindSerial);
        rewMsgBuf[99] = 0;
        systemConsoleMessage(rewMsgBuf);
        rewindSerials[curSavePos] = rewindSerial;

        // set up next rewind save
        // - don't clobber the current rewind position, unless it is the original top
        if (rewindPos == rewindTopPos) {
            rewindPos = curSavePos;
        }
        // - new identification and top
        rewindSerial++;
        rewindTopPos = curSavePos;
        // for the rest of the code, rewindTopPos will be where the newest rewind got stored
    }
}

void SetHomeConfigDir()
{
    //sprintf(homeConfigDir, "%s%s", get_xdg_user_config_home().c_str(), DOT_DIR);
    sprintf(homeConfigDir, "%s%s", "./" , DOT_DIR);
    struct stat s;
	if (stat(homeDataDir, &s) == -1 || !S_ISDIR(s.st_mode))

		Mod_Logln(homeConfigDir);
    //systemScreenMessage(homeConfigDir);
	mkdir(homeDataDir, 0755);
}

void SetHomeDataDir()
{
    //sprintf(homeDataDir, "%s%s", get_xdg_user_data_home().c_str(), DOT_DIR);
    sprintf(homeDataDir, "%s%s", "./", DOT_DIR);
    struct stat s;
    if (stat(homeDataDir, &s) == -1 || !S_ISDIR(s.st_mode))
		Mod_Logln(homeDataDir);
	mkdir(homeDataDir, 0755);
}


bool sdlLoadGameData(char* filepath) {
    int size = CPULoadRom(filepath);
    bool failed = (size == 0);
    if (!failed) {
        if (cpuSaveType == 0)
            utilGBAFindSave(size);
        else
            saveType = cpuSaveType;

        sdlApplyPerImagePreferences();

        doMirroring(mirroringEnable);

        cartridgeType = 0;
        emulator = GBASystem;

        CPUInit(biosFileNameGBA, useBios);
        int patchnum;
        for (patchnum = 0; patchnum < patchNum; patchnum++) {
            fprintf(stdout, "Trying patch %s%s\n", patchNames[patchnum],
                applyPatch(patchNames[patchnum], &rom, &size) ? " [success]" : "[failure]");
        }

        // Applying Mod Patches:
        if (Mod_betterEquipmentMenu) {
            Mod_Log("Applying menu patch... ");
            Mod_Logln(applyPatch(Mod_menuPatchPath, &rom, &size) ? " [success]" : "[failure]");
        }

        if (Mod_noHints) {
            Mod_Log("Applying hints patch... ");
            Mod_Logln(applyPatch(Mod_hintsPatchPath, &rom, &size) ? " [success]" : "[failure]");
        }



        CPUReset();
    }

    return failed;
}

bool sdlLoadGameData() { 
    return sdlLoadGameData(gameDataFile);
}


// PROGRAM ENTRY POINT!!
int main(int argc, char** argv)
{


    fprintf(stdout, "%s\n", VBA_NAME_AND_SUBVERSION);

    home = argv[0];
    //home = "./";
    SetHome(home);
    SetHomeConfigDir();
    SetHomeDataDir();

    frameSkip = 2;
    gbBorderOn = 0;

    parseDebug = true;

    gb_effects_config.stereo = 0.0;
    gb_effects_config.echo = 0.0;
    gb_effects_config.surround = false;
    gb_effects_config.enabled = false;


    LoadConfig(); 
	ReadOpts(argc, argv);// Parse command line arguments (overrides ini)

	LoadInputConfig();
	
	Mod_LoadModSettings();
    Mod_InitLog();


    if (!sdlCheckDirectory(screenShotDir))
        screenShotDir = NULL;
    if (!sdlCheckDirectory(saveDir))
        saveDir = NULL;
    if (!sdlCheckDirectory(batteryDir))
        batteryDir = NULL;

    //sdlSaveKeysSwitch = (ReadPrefHex("saveKeysSwitch"));
    sdlSaveKeysSwitch = (ReadPref("saveKeysSwitch"), 2);
    sdlOpenglScale = (ReadPrefHex("openGLscale"));

    if (optPrintUsage) {
        usage(argv[0]);
        exit(-1);
    }

    //if (!debugger) {
    //    if (optind >= argc) {
    //        systemMessage(0, "Missing image name");
    //        usage(argv[0]);
    //        exit(-1);
    //    }
    //}

	gameDataFile = Mod_romPath;

	if (optind < argc) {
		gameDataFile = argv[optind];
	}

	Mod_Logln(gameDataFile);
	Mod_Logln(filename);

    utilStripDoubleExtension(gameDataFile, filename);
    char* p = strrchr(filename, '.');

    if (p)
        *p = 0;

    if (autoPatch && patchNum == 0) {
        char* tmp;
        // no patch given yet - look for ROMBASENAME.ips
        tmp = (char*)malloc(strlen(filename) + 4 + 1);
        sprintf(tmp, "%s.ips", filename);
        patchNames[patchNum] = tmp;
        patchNum++;

        // no patch given yet - look for ROMBASENAME.ups
        tmp = (char*)malloc(strlen(filename) + 4 + 1);
        sprintf(tmp, "%s.ups", filename);
        patchNames[patchNum] = tmp;
        patchNum++;

        // no patch given yet - look for ROMBASENAME.ppf
        tmp = (char*)malloc(strlen(filename) + 4 + 1);
        sprintf(tmp, "%s.ppf", filename);
        patchNames[patchNum] = tmp;
        patchNum++;
    }

    soundInit();

    bool failed = true;

    IMAGE_TYPE type = utilFindType(gameDataFile);

    if (type == IMAGE_UNKNOWN) {
        systemMessage(0, "Unknown file type %s", gameDataFile);
        exit(-1);
    }
    cartridgeType = (int)type;

    //if (type == IMAGE_GB) {
    //    failed = !gbLoadRom(gameDataFile);
    //    if (!failed) {
    //        gbGetHardwareType();

    //        // used for the handling of the gb Boot Rom
    //        if (gbHardware & 7)
    //            gbCPUInit(biosFileNameGB, useBios);

    //        cartridgeType = IMAGE_GB;
    //        emulator = GBSystem;
    //        int size = gbRomSize, patchnum;
    //        for (patchnum = 0; patchnum < patchNum; patchnum++) {
    //            fprintf(stdout, "Trying patch %s%s\n", patchNames[patchnum],
    //                applyPatch(patchNames[patchnum], &gbRom, &size) ? " [success]" : "");
    //        }
    //        if (size != gbRomSize) {
    //            extern bool gbUpdateSizes();
    //            gbUpdateSizes();
    //            gbReset();
    //        }
    //        gbReset();
    //    }
    //} else if (type == IMAGE_GBA) {
    if (type == IMAGE_GBA) {
        failed = sdlLoadGameData(gameDataFile);
    }

    if (failed) {
        systemMessage(0, "Failed to load file %s", gameDataFile);
        Mod_Log("Failed to load file ");
        Mod_Logln(gameDataFile);
        exit(-1);
    }
    //} else {
    //    soundInit();
    //    cartridgeType = 0;
    //    strcpy(filename, "gnu_stub");
    //    rom = (uint8_t*)malloc(0x2000000);
    //    workRAM = (uint8_t*)calloc(1, 0x40000);
    //    bios = (uint8_t*)calloc(1, 0x4000);
    //    internalRAM = (uint8_t*)calloc(1, 0x8000);
    //    paletteRAM = (uint8_t*)calloc(1, 0x400);
    //    vram = (uint8_t*)calloc(1, 0x20000);
    //    oam = (uint8_t*)calloc(1, 0x400);
    //    pix = (uint8_t*)calloc(1, 4 * 241 * 162);
    //    ioMem = (uint8_t*)calloc(1, 0x400);

    //    emulator = GBASystem;

    //    CPUInit(biosFileNameGBA, useBios);
    //    CPUReset();
    //}

    sdlReadBattery();

    if (debugger)
        remoteInit();

    int flags = SDL_INIT_VIDEO | SDL_INIT_TIMER;

    if (SDL_Init(flags) < 0) {
        systemMessage(0, "Failed to init SDL: %s", SDL_GetError());
        exit(-1);
    }

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
        systemMessage(0, "Failed to init joystick support: %s", SDL_GetError());
    }

#if WITH_LIRC
    StartLirc();
#endif
    inputInitJoysticks();

    if (cartridgeType == IMAGE_GBA) {
        sizeX = 240;
        sizeY = 160;
        systemFrameSkip = frameSkip;
    } else if (cartridgeType == IMAGE_GB) {
        if (gbBorderOn) {
            sizeX = 256;
            sizeY = 224;
            gbBorderLineSkip = 256;
            gbBorderColumnSkip = 48;
            gbBorderRowSkip = 40;
        } else {
            sizeX = 160;
            sizeY = 144;
            gbBorderLineSkip = 160;
            gbBorderColumnSkip = 0;
            gbBorderRowSkip = 0;
        }
        systemFrameSkip = gbFrameSkip;
    } else {
        sizeX = 320;
        sizeY = 240;
    }

    sdlReadDesktopVideoMode();

    sdlInitVideo();

    //sdlSetWindowIcon(window);
    //filterFunction = initFilter(filter, systemColorDepth, sizeX);
    sdlSetFilter(filter);

    if (!filterFunction) {
        fprintf(stderr, "Unable to init filter '%s'\n", getFilterName(filter));
        exit(-1);
    }

    if (systemColorDepth == 15)
        systemColorDepth = 16;

    if (systemColorDepth != 16 && systemColorDepth != 24 && systemColorDepth != 32) {
        fprintf(stderr, "Unsupported color depth '%d'.\nOnly 16, 24 and 32 bit color depths are supported\n", systemColorDepth);
        exit(-1);
    }

    fprintf(stdout, "Color depth: %d\n", systemColorDepth);

    utilUpdateSystemColorMaps();

    if (delta == NULL) {
        delta = (uint8_t*)malloc(delta_size);
        memset(delta, 255, delta_size);
    }

    ifbFunction = initIFBFilter(ifbType, systemColorDepth);

    emulating = 1;
    renderedFrames = 0;

    autoFrameSkipLastTime = throttleLastTime = systemGetClock();

    // now we can enable cheats?
    {
        int i;
        for (i = 0; i < preparedCheats; i++) {
            const char* p;
            int l;
            p = preparedCheatCodes[i];
            l = strlen(p);
            if (l == 17 && p[8] == ':') {
                fprintf(stdout, "Adding cheat code %s\n", p);
                cheatsAddCheatCode(p, p);
            } else if (l == 13 && p[8] == ' ') {
                fprintf(stdout, "Adding CBA cheat code %s\n", p);
                cheatsAddCBACode(p, p);
            } else if (l == 8) {
                fprintf(stdout, "Adding GB(GS) cheat code %s\n", p);
                gbAddGsCheat(p, p);
            } else {
                fprintf(stderr, "Unknown format for cheat code %s\n", p);
            }
        }
    }

	Mod_Init(window, &glcontext);

    while (emulating) {
        if (should_video_resize.should) {
            sdlResizeVideo();
            should_video_resize.should = false;
        }

        if (!paused && active) {
            if (debugger && emulator.emuHasDebugger)
                remoteStubMain();
            else {
                emulator.emuMain(emulator.emuCount);
                if (rewindSaveNeeded && rewindMemory && emulator.emuWriteMemState) {
                    handleRewinds();
                }

                rewindSaveNeeded = false;
            }
        } else {
            //SDL_Delay(500);
			// Originally, the emulator wouldn't draw the screen when emulation is paused.
			// Since I'm developing a new pause menu, I need it to draw whether emulation is running or not.
			// This seems to be a decent fix to the problem. For now.
			systemDrawScreen();
			//SDL_Delay(50);

        }


        sdlPollEvents();
        if (should_update_fullscreen.should) {
            sdlSetFullScreen(should_update_fullscreen.val);
            should_update_fullscreen.should = false;
        }

		Mod_MainLoop();
#if WITH_LIRC
        lircCheckInput();
#endif
        if (mouseCounter) {
            mouseCounter--;
            if (mouseCounter == 0)
                SDL_ShowCursor(SDL_DISABLE);
        }
    }

    emulating = 0;
    fprintf(stdout, "Shutting down\n");
    remoteCleanUp();
    soundShutdown();

    if (openGL) {
        SDL_GL_DeleteContext(glcontext);
    }

    if (gbRom != NULL || rom != NULL) {
        sdlWriteBattery();
        emulator.emuCleanUp();
    }

    if (delta) {
        free(delta);
        delta = NULL;
    }

    if (filterPix) {
        free(filterPix);
        filterPix = NULL;
    }

    for (int i = 0; i < patchNum; i++) {
        free(patchNames[i]);
    }

#if WITH_LIRC
    StopLirc();
#endif

    SaveConfigFile();
    CloseConfig();
    SDL_Quit();
	Mod_OnExit();
    return 0;
}

bool ResetEmulation() {
	if (emulating) {
		emulator.emuReset();
		return true;
	}

	return false;
}

void systemMessage(int num, const char* msg, ...)
{
    (void)num; // unused params
    va_list valist;

    va_start(valist, msg);
    vfprintf(stderr, msg, valist);
    fprintf(stderr, "\n");
    va_end(valist);
}

void drawScreenMessage(uint8_t* screen, int pitch, int x, int y, unsigned int duration)
{
    if (screenMessage) {
        if (cartridgeType == 1 && gbBorderOn) {
            gbSgbRenderBorder();
        }
        if (((systemGetClock() - screenMessageTime) < duration) && !disableStatusMessages) {
            drawText(screen, pitch, x, y,
                screenMessageBuffer, false);
        } else {
            screenMessage = false;
        }
    }
}

void drawSpeed(uint8_t* screen, int pitch, int x, int y)
{
    char buffer[50];
    if (showSpeed == 1)
        sprintf(buffer, "%d%%", systemSpeed);
    else
        sprintf(buffer, "%3d%%(%d, %d fps)", systemSpeed,
            systemFrameSkip,
            showRenderedFrames);

    drawText(screen, pitch, x, y, buffer, showSpeedTransparent);
}


// Added by Alex - used for storing texture coords for aspect ratio correction:



// End of addition.


// Added by Alex - Correcting aspect ratio for OpenGL:
GLfloatRect getGLTextureCoords() {
    GLfloatRect retVal;

    retVal = {
        {0.0f, 0.0f},
        {destWidth / (GLfloat)textureSize, 0.0f},
        {0.0f, destHeight / (GLfloat)textureSize},
        {destWidth / (GLfloat)textureSize, destHeight / (GLfloat)textureSize}
    };

    return retVal;
}

// End of addition.

void systemDrawScreen()
{
    unsigned int destPitch = destWidth * (systemColorDepth >> 3);
    uint8_t* screen;

    renderedFrames++;

    if (openGL)
        screen = filterPix;
    else {
        screen = (uint8_t*)surface->pixels;
        SDL_LockSurface(surface);
    }

    if (ifbFunction)
        ifbFunction(pix + srcPitch, srcPitch, sizeX, sizeY);



    if (openGL && !(paused || show_options)) {
        int bytes = (systemColorDepth >> 3);
        for (int i = 0; i < sizeX; i++)
            for (int j = 0; j < sizeY + 1; j++) {
                 //I think this is trying to switch the Alpha and Red bytes of the color... Not sure though...
                uint8_t k;
                k = pix[i * bytes + j * srcPitch + 3];
                pix[i * bytes + j * srcPitch + 3] = pix[i * bytes + j * srcPitch + 1];
                pix[i * bytes + j * srcPitch + 1] = k;

                // wait... I don't think it's swapping it's colors correctly. let's experiment:

               /* uint8_t old0 = filterPix[i * bytes + j * destPitch + 0];
                uint8_t old1 = filterPix[i * bytes + j * destPitch + 1];
                uint8_t old2 = filterPix[i * bytes + j * destPitch + 2];
                uint8_t old3 = filterPix[i * bytes + j * destPitch + 3];

                filterPix[i * bytes + j * destPitch + 0] = old0;
                filterPix[i * bytes + j * destPitch + 1] = old0;
                filterPix[i * bytes + j * destPitch + 2] = old0;
                filterPix[i * bytes + j * destPitch + 3] = old0;*/

            }
    }

    filterFunction(pix + srcPitch, srcPitch, delta, screen,
        destPitch, sizeX, sizeY);


    // This was an attempt to handle some color/byte-order correction for OpenGL after the game image had been processed by the filter.
    //However, there was some color distortion in the results. I've determined that it's better to handle this correction BEFORE the image is filtered.
    /*
    if (openGL) {
        int bytes = (systemColorDepth >> 3);
        for (int i = 0; i < destWidth; i++)
            for (int j = 0; j < destHeight; j++) {

                uint8_t k;
                k = filterPix[i * bytes + j * destPitch + 3];
                filterPix[i * bytes + j * destPitch + 3] = filterPix[i * bytes + j * destPitch + 1];
                filterPix[i * bytes + j * destPitch + 1] = k;
            }
    }*/

    drawScreenMessage(screen, destPitch, 10, destHeight - 20, 3000);

    if (showSpeed && fullScreen)
        drawSpeed(screen, destPitch, 10, 20);

    if (openGL) {

		//if (!paused) {

        glRect = getGLTextureCoords();
		glClear(GL_COLOR_BUFFER_BIT);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, destWidth);
		if (systemColorDepth == 16)
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, destWidth, destHeight,
				GL_RGB, GL_UNSIGNED_SHORT_5_6_5, screen);
		else
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, destWidth, destHeight,
				//GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, screen);
				GL_RGBA, GL_UNSIGNED_BYTE, screen);

		glBegin(GL_TRIANGLE_STRIP);
		//glTexCoord2f(0.0f, 0.0f);
		glTexCoord2f(glRect.top_left.x, glRect.top_left.y);
		glVertex3i(0, 0, 0);
		//glTexCoord2f(destWidth / (GLfloat)textureSize, 0.0f);
		glTexCoord2f(glRect.top_right.x, glRect.top_right.y);
		glVertex3i(1, 0, 0);
		//glTexCoord2f(0.0f, destHeight / (GLfloat)textureSize);
		glTexCoord2f(glRect.bottom_left.x, glRect.bottom_left.y);
		glVertex3i(0, 1, 0);
		//glTexCoord2f(destWidth / (GLfloat)textureSize, destHeight / (GLfloat)textureSize);
		glTexCoord2f(glRect.bottom_right.x, glRect.bottom_right.y);
		glVertex3i(1, 1, 0);



		glEnd();
		Mod_DrawDisplay(texture, surface, window, renderer);
		SDL_GL_SwapWindow(window);

		//}
		//else {

		//}



    } else {
        SDL_UnlockSurface(surface);

		

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

		//Mod_DrawDisplay(texture, surface, window, renderer);

        SDL_RenderPresent(renderer);

		//Mod_DrawDisplay(texture, surface, window, renderer);
    }
}

void systemSetTitle(const char* title)
{
    SDL_SetWindowTitle(window, title);
}

void systemShowSpeed(int speed)
{
    systemSpeed = speed;

    showRenderedFrames = renderedFrames;
    renderedFrames = 0;

    if (!fullScreen && showSpeed) {
        char buffer[80];
        if (showSpeed == 1)
            sprintf(buffer, "VBA-M - %d%%", systemSpeed);
        else
            sprintf(buffer, "VBA-M - %d%%(%d, %d fps)", systemSpeed,
                systemFrameSkip,
                showRenderedFrames);

        systemSetTitle(buffer);
    }
}

void systemFrame()
{
}

void system10Frames(int rate)
{
    uint32_t time = systemGetClock();
    if (!wasPaused && autoFrameSkip) {
        uint32_t diff = time - autoFrameSkipLastTime;
        int speed = 100;

        if (diff)
            speed = (1000000 / rate) / diff;

        if (speed >= 98) {
            frameskipadjust++;

            if (frameskipadjust >= 3) {
                frameskipadjust = 0;
                if (systemFrameSkip > 0)
                    systemFrameSkip--;
            }
        } else {
            if (speed < 80)
                frameskipadjust -= (90 - speed) / 5;
            else if (systemFrameSkip < 9)
                frameskipadjust--;

            if (frameskipadjust <= -2) {
                frameskipadjust += 2;
                if (systemFrameSkip < 9)
                    systemFrameSkip++;
            }
        }
    }
    if (rewindMemory) {
        if (++rewindCounter >= rewindTimer) {
            rewindSaveNeeded = true;
            rewindCounter = 0;
        }
    }

    if (systemSaveUpdateCounter) {
        if (--systemSaveUpdateCounter <= SYSTEM_SAVE_NOT_UPDATED) {
            sdlWriteBattery();
            systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
        }
    }

    wasPaused = false;
    autoFrameSkipLastTime = time;
}

void systemScreenCapture(int a)
{
    char buffer[2048];
    bool result = false;
    char *gameDir = sdlGetFilePath(filename);
    char *gameFile = sdlGetFilename(filename);

    if (captureFormat) {
        if (screenShotDir)
            sprintf(buffer, "%s%c%s%02d.bmp", screenShotDir, FILE_SEP, gameFile, a);
        else if (access(gameDir, W_OK) == 0)
            sprintf(buffer, "%s%c%s%02d.bmp", gameDir, FILE_SEP, gameFile, a);
        else
            sprintf(buffer, "%s%c%s%02d.bmp", homeDataDir, FILE_SEP, gameFile, a);

        result = emulator.emuWriteBMP(buffer);
    } else {
        if (screenShotDir)
            sprintf(buffer, "%s%c%s%02d.png", screenShotDir, FILE_SEP, gameFile, a);
        else if (access(gameDir, W_OK) == 0)
            sprintf(buffer, "%s%c%s%02d.png", gameDir, FILE_SEP, gameFile, a);
        else
            sprintf(buffer, "%s%c%s%02d.png", homeDataDir, FILE_SEP, gameFile, a);

        result = emulator.emuWritePNG(buffer);
    }

    if (result)
	systemScreenMessage("Screen capture");

    freeSafe(gameFile);
    freeSafe(gameDir);
}

void systemSaveOldest()
{
    // I need to be implemented
}

void systemLoadRecent()
{
    // I need to be implemented
}

uint32_t systemGetClock()
{
    return SDL_GetTicks();
}

void systemGbPrint(uint8_t* data, int len, int pages, int feed, int palette, int contrast)
{
    (void)data; // unused params
    (void)len; // unused params
    (void)pages; // unused params
    (void)feed; // unused params
    (void)palette; // unused params
    (void)contrast; // unused params
}

/* xKiv: added timestamp */
void systemConsoleMessage(const char* msg)
{
    time_t now_time;
    struct tm now_time_broken;

    now_time = time(NULL);
    now_time_broken = *(localtime(&now_time));
    fprintf(
        stdout,
        "%02d:%02d:%02d %02d.%02d.%4d: %s\n",
        now_time_broken.tm_hour,
        now_time_broken.tm_min,
        now_time_broken.tm_sec,
        now_time_broken.tm_mday,
        now_time_broken.tm_mon + 1,
        now_time_broken.tm_year + 1900,
        msg);
}

void systemScreenMessage(const char* msg)
{

    screenMessage = true;
    screenMessageTime = systemGetClock();
    if (strlen(msg) > 20) {
        strncpy(screenMessageBuffer, msg, 20);
        screenMessageBuffer[20] = 0;
    } else
        strcpy(screenMessageBuffer, msg);

    systemConsoleMessage(msg);
}

bool systemCanChangeSoundQuality()
{
    return false;
}

bool systemPauseOnFrame()
{
    if (pauseNextFrame) {
        paused = true;
        pauseNextFrame = false;
        return true;
    }
    return false;
}

void systemGbBorderOn()
{
    sizeX = 256;
    sizeY = 224;
    gbBorderLineSkip = 256;
    gbBorderColumnSkip = 48;
    gbBorderRowSkip = 40;

    sdlInitVideo();

    filterFunction = initFilter(filter, systemColorDepth, sizeX);
}

bool systemReadJoypads()
{
    return true;
}

uint32_t systemReadJoypad(int which)
{
    return inputReadJoypad(which);
}
//static uint8_t sensorDarkness = 0xE8; // total darkness (including daylight on rainy days)

void systemUpdateSolarSensor()
{
}

void systemCartridgeRumble(bool)
{
}

void systemUpdateMotionSensor()
{
    inputUpdateMotionSensor();
    systemUpdateSolarSensor();
}

int systemGetSensorX()
{
    return inputGetSensorX();
}

int systemGetSensorY()
{
    return inputGetSensorY();
}

int systemGetSensorZ()
{
    return 0;
}

uint8_t systemGetSensorDarkness()
{
    return 0xE8;
}

SoundDriver* systemSoundInit()
{
    soundShutdown();

    return new SoundSDL();
}

void systemOnSoundShutdown()
{
}

void systemOnWriteDataToSoundBuffer(const uint16_t* finalWave, int length)
{
    (void)finalWave; // unused params
    (void)length; // unused params
}

void log(const char* defaultMsg, ...)
{
    static FILE* out = NULL;

    if (out == NULL) {
        out = fopen("trace.log", "w");
    }

    va_list valist;

    va_start(valist, defaultMsg);
    vfprintf(out, defaultMsg, valist);
    va_end(valist);
}
