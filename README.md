# Metroid: Zero Mission - PC Edition:
This is project is an attempt to port 2004's "Metroid: Zero Mission" for the GameBoy Advance to modern PCs.
This was done by integrating the original game data into the SDL2 version of VisualBoyAdvance-M, modifying both
to add new features and more seamless experience.

This readme is still a work in progress. Stay tuned.

## Building instructions:
I intended for this project to be run on a 64-bit architecture. I have no idea if you can build it for 32-bit.

When generating your build folder from cmake, you need to use these cmake arguments as well:
* __-DENABLE_WX=OFF__: We're not using the wxWidgets build of VBA-M. However, CMakeLists.txt has it ON by default.
    For this project, however, adding it is a waste of compile-time, and will probably result in errors. 
    
* __-DENABLE_SDL=ON__: THIS is the version of VBA-M that the project uses. However, cmake won't generate it by default.
    We need to tell it to do so manually.


Generally, you should simply follow the building instructions in the original VBA-M readme (below), but there are a few specific notes:

### Windows:
If building on Windows (for Windows), you're going to use <a href="#Visual Studio Support">Visual Studio Support</a>,
discussed below in the original readme, but with the additional CMake arguments from above.

Overall, the build commands for Windows (Visual Studio) will look something like this:

```
mkdir build
cd build
cmake .. -DVCPKG_TARGET_TRIPLET=x64-windows -DENABLE_WX=OFF -DENABLE_SDL=ON
msbuild -m -p:BuildInParallel=true -p:Configuration=Release .\ALL_BUILD.vcxproj 
```

The original instructions say that building the dependencies doesn't currently work in VS-2019, and that you'll
have to do that part in VS-2017 instead. I didn't seem to have this problem, but maybe that's because I had some of
them already built. Your milage may vary.


Additionally, the contents of ./exec_assets/windows need to be copied into the same directory as the built executable
in order for the program to run (I haven't gotten around to automating that yet). This folder includes a few additional
required .dlls, the actual gamedata files, a copy of the basic config file, and a font asset.

### Other Platforms:
I can't think of anything in my code that would prevent VBA-M from compiling on other platforms, using the instructions
from the original readme, but I haven't tried it. You're milage may vary.

You will still need the gamedata files, config file, and font asset to go with the executable. You can find them in
./exec_assets/other



<hr/>


## Original VBA-M Readme:
<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->


- [Visual Boy Advance - M](#visual-boy-advance---m)
  - [Building](#building)
  - [Building a Libretro core](#building-a-libretro-core)
  - [Visual Studio Support](#visual-studio-support)
  - [Dependencies](#dependencies)
  - [Cross compiling for 32 bit on a 64 bit host](#cross-compiling-for-32-bit-on-a-64-bit-host)
  - [Cross Compiling for Win32](#cross-compiling-for-win32)
  - [CMake Options](#cmake-options)
  - [MSys2 Notes](#msys2-notes)
  - [Debug Messages](#debug-messages)
  - [Reporting Crash Bugs](#reporting-crash-bugs)
  - [Contributing](#contributing)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

[![Join the chat at https://gitter.im/visualboyadvance-m/Lobby](https://badges.gitter.im/visualboyadvance-m/Lobby.svg)](https://gitter.im/visualboyadvance-m/Lobby)
[![travis](https://travis-ci.org/visualboyadvance-m/visualboyadvance-m.svg?branch=master)](https://travis-ci.org/visualboyadvance-m/visualboyadvance-m)
[![appveyor](https://ci.appveyor.com/api/projects/status/5ckx25vct1q1ovfc?svg=true)](https://ci.appveyor.com/project/ZachBacon65337/visualboyadvance-m-2ys5r)

# Visual Boy Advance - M

Game Boy Advance Emulator

The Forum is here: https://vba-m.com/forums/

Windows and Mac builds are in the [releases tab](https://github.com/visualboyadvance-m/visualboyadvance-m/releases).

Daily Ubuntu packages here: https://code.launchpad.net/~sergio-br2/+archive/ubuntu/vbam-trunk

Your distribution may have packages available as well, search for "vbam" or "visualboyadvance-m".

It is also generally very easy to build from source, see below.

If you are using the windows binary release and you need localization, unzip
the `translations.zip` to the same directory as the executable.

If you are having issues, try resetting the config file first, go to `Help -> Factory Reset`.

## Building

The basic formula to build vba-m is:

```shell
cd ~ && mkdir src && cd src
git clone https://github.com/visualboyadvance-m/visualboyadvance-m.git
cd visualboyadvance-m
./installdeps

# ./installdeps will give you build instructions, which will be similar to:

mkdir build && cd build
cmake ..
make -j`nproc`
```

`./installdeps` is supported on MSys2, Linux (Debian/Ubuntu, Fedora, Arch,
Solus, OpenSUSE, Gentoo and RHEL/CentOS) and Mac OS X (homebrew, macports or
fink.)

The Ninja cmake generator is also now supported (except for Visual Studio.)

## Building a Libretro core

```
Clone this repo and then,
$ cd src
$ cd libretro
$ make

Copy vbam_libretro.so to your RetroArch cores directory.
```

## Visual Studio Support

For visual studio, dependency management is handled automatically with vcpkg,
just clone the repository with git and build with cmake. You can do this from
the developer command line as well. 2019 will not work yet for building
dependencies, but you can build the dependencies in 2017 and then use the
project from 2019.

Using your own user-wide installation of vcpkg is supported, just make sure the
environment variable `VCPKG_ROOT` is set.

To build in the visual studio command prompt, use something like this:

```
mkdir build
cd build
cmake .. -DVCPKG_TARGET_TRIPLET=x64-windows
msbuild -m -p:BuildInParallel=true -p:Configuration=Release .\ALL_BUILD.vcxproj 
```

This support is new and we are still working out some issues, including support
for static builds.

## Dependencies

If your OS is not supported, you will need the following:

- C++ compiler and binutils
- [make](https://en.wikipedia.org/wiki/Make_(software))
- [CMake](https://cmake.org/)
- [git](https://git-scm.com/)
- [nasm](https://www.nasm.us/) (optional, for 32 bit builds)

And the following development libraries:

- [zlib](https://zlib.net/) (required)
- [mesa](https://mesa3d.org/) (if using X11 or any OpenGL otherwise)
- [ffmpeg](https://ffmpeg.org/) (optional, at least version `4.0.4`, for game recording)
- [gettext](https://www.gnu.org/software/gettext/) and gettext-tools (optional, with ENABLE_NLS)
- [libpng](http://www.libpng.org/pub/png/libpng.html) (required)
- [SDL2](https://www.libsdl.org/) (required)
- [SFML](https://www.sfml-dev.org/) (optional, for link)
- [OpenAL](https://www.openal.org/) or [openal-soft](https://kcat.strangesoft.net/openal.html) (optional, a sound interface)
- [wxWidgets](https://wxwidgets.org/) (required for GUI, 2.8 is still supported, --enable-stl is supported)

On Linux and similar, you also need the version of GTK your wxWidgets is linked
to (usually 2 or 3) and the xorg development libraries.

Support for more OSes/distributions for `./installdeps` is planned.

## Cross compiling for 32 bit on a 64 bit host

`./installdeps m32` will set things up to build a 32 bit binary.

This is supported on Fedora, Arch, Solus and MSYS2.

## Cross Compiling for Win32

`./installdeps` takes one optional parameter for cross-compiling target, which
may be `win32` which is an alias for `mingw-w64-i686` to target 32 bit Windows,
or `mingw-gw64-x86_64` for 64 bit Windows targets.

The target is implicit on MSys2 depending on which MINGW shell you started (the
value of `$MSYSTEM`.)

On Debian/Ubuntu this uses the MXE apt repository and works quite well.

On Fedora it can build using the Fedora MinGW packages, albeit with wx 2.8, no
OpenGL support, and no Link support for lack of SFML.

On Arch it currently doesn't work at all because the AUR stuff is completely
broken, I will at some point redo the arch stuff to use MXE as well.

## CMake Options

The CMake code tries to guess reasonable defaults for options, but you can
override them, for example:

```shell
cmake .. -DENABLE_LINK=NO
```

Of particular interest is making **RELEASE** or **DEBUG** builds, the default
mode is **RELEASE**, to make a **DEBUG** build use something like:

```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

Here is the complete list:

| **CMake Option**      | **What it Does**                                                     | **Defaults**          |
|-----------------------|----------------------------------------------------------------------|-----------------------|
| ENABLE_SDL            | Build the SDL port                                                   | OFF                   |
| ENABLE_WX             | Build the wxWidgets port                                             | ON                    |
| ENABLE_DEBUGGER       | Enable the debugger                                                  | ON                    |
| ENABLE_NLS            | Enable translations                                                  | ON                    |
| ENABLE_ASM_CORE       | Enable x86 ASM CPU cores (**BUGGY AND DANGEROUS**)                   | OFF                   |
| ENABLE_ASM            | Enable the following two ASM options                                 | ON for 32 bit builds  |
| ENABLE_ASM_SCALERS    | Enable x86 ASM graphic filters                                       | ON for 32 bit builds  |
| ENABLE_MMX            | Enable MMX                                                           | ON for 32 bit builds  |
| ENABLE_LINK           | Enable GBA linking functionality (requires SFML)                     | AUTO                  |
| ENABLE_LIRC           | Enable LIRC support                                                  | OFF                   |
| ENABLE_FFMPEG         | Enable ffmpeg A/V recording                                          | AUTO                  |
| ENABLE_ONLINEUPDATES  | Enable online update checks                                          | ON                    |
| ENABLE_LTO            | Compile with Link Time Optimization (gcc and clang only)             | ON for release build  |
| ENABLE_GBA_LOGGING    | Enable extended GBA logging                                          | ON                    |
| ENABLE_DIRECT3D       | Direct3D rendering for wxWidgets (Windows, **NOT IMPLEMENTED!!!**)   | ON                    |
| ENABLE_XAUDIO2        | Enable xaudio2 sound output for wxWidgets (Windows only)             | ON                    |
| ENABLE_OPENAL         | Enable OpenAL for the wxWidgets port                                 | AUTO                  |
| ENABLE_SSP            | Enable gcc stack protector support (gcc only)                        | OFF                   |
| ENABLE_ASAN           | Enable libasan sanitizers (by default address, only in debug mode)   | OFF                   |
| VBAM_STATIC           | Try link all libs statically (the following are set to ON if ON)     | OFF                   |
| SDL2_STATIC           | Try to link static SDL2 libraries                                    | OFF                   |
| SFML_STATIC_LIBRARIES | Try to link static SFML libraries                                    | OFF                   |
| FFMPEG_STATIC         | Try to link static ffmpeg libraries                                  | OFF                   |
| SSP_STATIC            | Try to link static gcc stack protector library (gcc only)            | OFF except Win32      |
| OPENAL_STATIC         | Try to link static OpenAL libraries                                  | OFF                   |
| SSP_STATIC            | Link gcc stack protecter libssp statically (gcc, with ENABLE_SSP)    | OFF                   |

Note for distro packagers, we use the CMake module
[GNUInstallDirs](https://cmake.org/cmake/help/v2.8.12/cmake.html#module:GNUInstallDirs)
to configure installation directories.

## MSys2 Notes

To run the resulting binary, you can simply type:

```shell
./visualboyadvance-m
```

in the shell where you built it.

If you built with `-DCMAKE_BUILD_TYPE=Debug`, you will get a console app and
will see debug messages, even in mintty.

If you want to start the binary from e.g. a shortcut or Explorer, you will need
to put `c:\msys64\mingw32\bin` for 32 bit builds and `c:\msys64\mingw64\bin`
for 64 bit builds in your PATH (to edit system PATH, go to Control Panel ->
System -> Advanced system settings -> Environment Variables.)

If you want to package the binary, you will need to include the MinGW DLLs it
depends on, they can install to the same directory as the binary.

For our own builds, we use MXE to make static builds.

## Debug Messages

We have an override for `wxLogDebug()` to make it work even in non-debug builds
of wx and on windows, even in mintty. Using this function for console debug
messages is recommended.

It works like `printf()`, e.g.:

```cpp
int foo = 42;
wxLogDebug(wxT("the value of foo = %d"), foo);
```

## Reporting Crash Bugs

If the emulator crashes and you wish to report the bug, a backtrace made with
debug symbols would be immensely helpful.

To generate one (on Linux and MSYS2) first build in debug mode by invoking
`cmake` as:

```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

After you've reproduced the crash, you need the core dump file, you may need to
do something such as:

```shell
ulimit -c unlimited
```

in your shell to enable coredump files.

[This
post](https://ask.fedoraproject.org/en/question/98776/where-is-core-dump-located/?answer=98779#post-id-98779)
explains how to retrieve core dump on Fedora Linux (and possibly other
distributions.)

Once you have the core dump file, open it with `gdb`, for example:

```shell
gdb -c core ./visualboyadvance-m
```

In the `gdb` shell, to print the backtrace, type:

```
bt
```

This may be a bit of a hassle, but it helps us out immensely.

## Contributing

Please keep in mind that this app needs to run on Windows, Linux and macOS at
the very least, so code should be portable and/or use the appropriate `#ifdef`s
and the like when needed.

Please try to craft a good commit message, this post by the great tpope explains
how to do so:
http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html

If you have multiple small commits for a change, please try to use `git rebase
-i` (interactive rebase) to squash them into one or a few logical commits (with
good commit messages!) See:
https://git-scm.com/book/en/v2/Git-Tools-Rewriting-History if you are new to
this.
