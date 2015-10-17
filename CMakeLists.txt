cmake_minimum_required(VERSION 3.3)

project(simple-platformer-build)

include(ExternalProject)
set(DEPS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps-install")
ExternalProject_Add(Corrade
	GIT_REPOSITORY "git://github.com/mosra/corrade.git"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		"-DBUILD_STATIC=ON"
		"-DBUILD_SHARED_LIBS=OFF"
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(Magnum
	DEPENDS Corrade
	GIT_REPOSITORY "git://github.com/mosra/magnum.git"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		"-DBUILD_STATIC=ON"
		"-DBUILD_SHARED_LIBS=OFF"
		"-DWITH_SDL2APPLICATION=ON"
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(simple-platformer
	DEPENDS Magnum
	SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		"-DCMAKE_MODULE_PATH=${CMAKE_CURRENT_BINARY_DIR}/Magnum-prefix/src/Magnum/modules"
	INSTALL_COMMAND ""
)
