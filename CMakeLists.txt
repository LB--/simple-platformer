cmake_minimum_required(VERSION 3.4)

project(simple-platformer-build
	LANGUAGES C CXX
)

set(CMAKE_C_FLAGS "" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "" CACHE STRING "C++ flags")
set(SDL2_VERSION "2.0.7" CACHE STRING "The version of SDL2 to use")

set(_global_args
	"-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
	"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"

	"-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
	"-DCMAKE_C_STANDARD=11"
	"-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"

	"-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
	"-DCMAKE_CXX_STANDARD=14"
	"-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"

	"-DSDL2_INCLUDE_DIR=${CMAKE_INSTALL_PREFIX}/include/SDL2"
)

include(ExternalProject)
ExternalProject_Add(Corrade
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/corrade"
	BUILD_ALWAYS 1
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		${_global_args}
		"-DBUILD_DEPRECATED=OFF"
		${CUSTOM_CORRADE_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
if(MSVC OR MINGW)
	if(MSVC)
		set(_sdl2_url "https://www.libsdl.org/release/SDL2-devel-${SDL2_VERSION}-VC.zip")
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(_sdl2_install
				cmake -E copy_directory "include" "${CMAKE_INSTALL_PREFIX}/include/SDL2" &&
				cmake -E copy_directory "lib/x86" "${CMAKE_INSTALL_PREFIX}/lib" &&
				cmake -E copy "lib/x86/SDL2.dll" "${CMAKE_INSTALL_PREFIX}/bin/SDL2.dll"
			)
		elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_sdl2_install
				cmake -E copy_directory "include" "${CMAKE_INSTALL_PREFIX}/include/SDL2" &&
				cmake -E copy_directory "lib/x64" "${CMAKE_INSTALL_PREFIX}/lib" &&
				cmake -E copy "lib/x64/SDL2.dll" "${CMAKE_INSTALL_PREFIX}/bin/SDL2.dll"
			)
		endif()
	elseif(MINGW)
		set(_sdl2_url "https://www.libsdl.org/release/SDL2-devel-${SDL2_VERSION}-mingw.tar.gz")
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(_sdl2_install
				cmake -E copy_directory "i686-w64-mingw32" "${CMAKE_INSTALL_PREFIX}"
			)
		elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_sdl2_install
				cmake -E copy_directory "x86_64-w64-mingw32" "${CMAKE_INSTALL_PREFIX}"
			)
		endif()
	endif()
	if(NOT _sdl2_install)
		message(FATAL_ERROR "Could not detect whether this platform is 32 or 64 bit")
	endif()

	ExternalProject_Add(SDL2
		URL ${_sdl2_url}
		TLS_VERIFY 1
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ${_sdl2_install}
	)
else()
	ExternalProject_Add(SDL2
		URL "https://www.libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz"
		TLS_VERIFY 1
		BUILD_ALWAYS 1
		LIST_SEPARATOR "#"
		CMAKE_ARGS
			${_global_args}
			"-DBUILD_SHARED_LIBS=ON"
			"-DSDL_SHARED=ON"
			"-DSDL_STATIC=OFF"
			${CUSTOM_SDL2_ARGS}
		TEST_EXCLUDE_FROM_MAIN 1
	)
endif()
ExternalProject_Add(Magnum
	DEPENDS
		Corrade
		SDL2
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/magnum"
	BUILD_ALWAYS 1
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		${_global_args}
		"-DBUILD_DEPRECATED=OFF"
		"-DWITH_MESHTOOLS=ON"
		"-DWITH_PRIMITIVES=ON"
		"-DWITH_SCENEGRAPH=ON"
		"-DWITH_SHADERS=ON"
		"-DWITH_SDL2APPLICATION=ON"
		"-DWITH_TEXT=ON"
		${CUSTOM_MAGNUM_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
set(_freetype_shared ON)
if(WIN32 AND NOT MINGW)
	set(_freetype_shared OFF)
endif()
ExternalProject_Add(FreeType
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype2"
	BUILD_ALWAYS 1
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		${_global_args}
		"-DBUILD_SHARED_LIBS=${_freetype_shared}"
		${CUSTOM_FREETYPE_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(MagnumPlugins
	DEPENDS
		Corrade
		Magnum
		FreeType
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/magnum-plugins"
	BUILD_ALWAYS 1
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		${_global_args}
		"-DBUILD_DEPRECATED=OFF"
		"-DWITH_FREETYPEFONT=ON"
		${CUSTOM_MAGNUMPLUGINS_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(MagnumExtras
	DEPENDS
		Corrade
		Magnum
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/magnum-extras"
	BUILD_ALWAYS 1
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		${_global_args}
		"-DBUILD_DEPRECATED=OFF"
		"-DWITH_UI=ON"
#		"-DWITH_UI_GALLERY=ON"
		${CUSTOM_MAGNUMEXTRAS_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(simple-platformer
	DEPENDS
		Corrade
		Magnum
		MagnumPlugins
		MagnumExtras
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src"
	BUILD_ALWAYS 1
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		${_global_args}
		"-DCMAKE_MODULE_PATH=${CMAKE_INSTALL_PREFIX}#${CMAKE_CURRENT_SOURCE_DIR}/deps/magnum/modules"
		${CUSTOM_SIMPLEPLATFORMER_ARGS}
	TEST_AFTER_INSTALL 1
	TEST_COMMAND
		ctest -VV
)
