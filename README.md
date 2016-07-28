Simple Platformer [![travis](https://travis-ci.org/LB--/simple-platformer.png?branch=main)](https://travis-ci.org/LB--/simple-platformer) [![gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/LB--/simple-platformer?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
=================

Just a simple C++14 3D platformer in the public domain, using [Magnum](https://github.com/mosra/magnum).

## Building
Building is done via the CMake superproject in the root directory.
It automatically builds Corrade, Magnum, FreeType, Magnum Plugins, and Magnum Extras for you.
It will also download, and in some cases build, development libraries for SDL2.
If you already have all those, just build from the `src` directory instead.

Typical build script:
```sh
mkrdir build && cd build
cmake -G "MinGW Makefiles" .. -DCMAKE_INSTALL_PREFIX="where/you/want/everything"
cmake --build .
```
After that, the `simple-platformer` executable will be in `where/you/want/everything/bin/`.
For continuous development, just keep running `cmake --build .` - only things which have changed will be rebuilt.

### Building on Windows
On Windows it is relatively simple to build with either MinGW or Visual Studio.

#### Using MinGW
You need at least GCC 6.
I personally recommend using [nuwen MinGW](http://nuwen.net/mingw.html).
If you do, you may need to rename or remove the SDL-related directories and files so that CMake doesn't try to use them instead - they are built statically and that is not currently supported.
Then, just build with CMake as directed above.

#### Using MSVC / Visual Studio
If you want to use Visual Studio, you will need at least Visual Studio 2015, as that is the earliest version supported by Magnum.

If you are using CMake 3.6.1 or lower, you may get the dreaded `Could NOT find Freetype (missing: FREETYPE_LIBRARY) (found version "2.6.5")`.
In this case, just tell it about the exact full path the the Freetype lib file, e.g. `"-DCUSTOM_MAGNUMPLUGINS_ARGS=-DFREETYPE_LIBRARY=C:/where/you/want/everything/lib/freetyped.lib"`.
Alternatively, use a version of CMake newer than 3.6.1 to make use of the bugfix.

Build with CMake as instructed above using either `-G "Visual Studio 14 2015"` or `-G "Visual Studio 14 2015 Win64"`.

### Building on Linux
You need at least GCC 6.
Clang should also work just fine but is untested.
It should be as simple as following the CMake build instructions above but without specifying any generator, or by specifying the generator of your choice.
