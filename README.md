Simple Platformer [![travis](https://travis-ci.org/LB--/simple-platformer.png?branch=main)](https://travis-ci.org/LB--/simple-platformer) [![gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/LB--/simple-platformer?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
=================

Just a simple C++14 3D platformer in the public domain, using [Magnum](https://github.com/mosra/magnum).

## Building

Building is done via the CMake superproject in the root directory.
It automatically builds shared versions of Corrade, Magnum, FreeType, and Magnum Plugins for you.
If you already have those, just build from the `src` directory instead.

Either way, you need to have SDL2 shared devel libraries already installed, as well as any other dependencies Magnum requires for your particular system.

Typical build script:
```sh
mkrdir build && cd build
cmake -G "MinGW Makefiles" .. -DCMAKE_INSTALL_PREFIX="where/you/want/everything"
cmake --build .
```
After that, the `simple-platformer` executable will be in `where/you/want/everything/bin/`.
For continuous development, just keep running `cmake --build .` - only things which have changed will be rebuilt.

### Building on Windows

This should be relatively painless if you use [nuwen MinGW](http://nuwen.net/mingw.html).
After installing nuwen MinGW, visit `C:/MinGW/bin` and if `ming32-make` isn't already there, copy or symlink `make` to `mingw32-make` so that the MinGW CMake generator is available.
Nuwen MinGW only ships with static SDL2 libraries, so you need to [download the development libraries for MinGW](https://www.libsdl.org/download-2.0.php) - choose `SDL2-devel-2.y.z-mingw.tar.gz` and extract with your archive program of choice (e.g. 7zip or WinRAR).
Then, add the correct directory to your PATH environment variable (or include in `CMAKE_PREFIX_PATH`).
Specifically, you want the correct lib folder: `lib/x64` or `lib/x86`.
Also, you need to set `"-DCUSTOM_SIMPLEPLATFORMER_ARGS=-DSDL2_INCLUDE_DIR=C:/path/to/SDL2/include"`
Then, just build with CMake as directed above.

#### Using MSVC / Visual Studio

If you want to use Visual Studio, you will need at least Visual Studio 2015, as that is the earliest version supported by Magnum.
You will also need to [download the SDL2 development libraries for MSVC](https://www.libsdl.org/download-2.0.php) - choose `SDL2-devel-2.y.z-VC.zip` and extract it.
Then, add the correct directory to your PATH environment variable (or include in `CMAKE_PREFIX_PATH`).
Specifically, you want the correct lib folder: `lib/x64` or `lib/x86`.
Also, you need to set `"-DCUSTOM_SIMPLEPLATFORMER_ARGS=-DSDL2_INCLUDE_DIR=C:/path/to/SDL2/include"`

This project also requires FreeType to be built as a shared library rather than a static library.
However, if you look at FreeType's `CMakeLists.txt`, you will see this wonderful nugget of insight:
```cmake
if (WIN32 AND NOT MINGW AND BUILD_SHARED_LIBS)
  message(FATAL_ERROR "Building shared libraries on Windows needs MinGW")
endif ()
```
Despite what the message says, [it is possible to use Visual Studio to build the shared library](http://stackoverflow.com/a/7387618/1959975).
Follow the instructions at that link and then ensure that the DLL and lib files are present in the PATH environment variable.
(Or include the path to them in `CMAKE_PREFIX_PATH`)

For some reason building with MSVC makes `find_package` less smart, and you may have to battle the dreaded `Could NOT find Freetype (missing: FREETYPE_LIBRARY) (found version "2.6.1")`.
If you're having trouble, put your CMake build log on https://gist.github.com/ and [ask for help in chat](https://gitter.im/LB--/simple-platformer).

### Building on Linux

Look at the `.travis.yml` file for hints.
If you get it working on your system, consider adding helpful instructions to this readme!
