cmake_minimum_required(VERSION 3.3)

project(simple-platformer-build)

include(ExternalProject)
set(DEPS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps-install")
ExternalProject_Add(Corrade
	GIT_REPOSITORY "git://github.com/mosra/corrade.git"
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		${CUSTOM_CORRADE_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(Magnum
	DEPENDS
		Corrade
	GIT_REPOSITORY "git://github.com/mosra/magnum.git"
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		"-DWITH_SDL2APPLICATION=ON"
		"-DWITH_SHADERS=ON"
		"-DWITH_TEXT=ON"
		${CUSTOM_MAGNUM_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
if(NOT MSVC)
	set(FREETYPE_TARGET "FreeType")
	ExternalProject_Add(${FREETYPE_TARGET}
		GIT_REPOSITORY "git://git.sv.gnu.org/freetype/freetype2.git"
		LIST_SEPARATOR "#"
		CMAKE_ARGS
			"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
			"-DBUILD_SHARED_LIBS=ON"
			${CUSTOM_FREETYPE_ARGS}
		TEST_EXCLUDE_FROM_MAIN 1
	)
else()
	message(WARNING "FreeType needs to be built as shared, not static - see http://stackoverflow.com/a/7387618/1959975 for help")
endif()
ExternalProject_Add(MagnumPlugins
	DEPENDS
		Corrade
		Magnum
		${FREETYPE_TARGET}
	GIT_REPOSITORY "git://github.com/mosra/magnum-plugins.git"
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		"-DWITH_FREETYPEFONT=ON"
		${CUSTOM_MAGNUMPLUGINS_ARGS}
	TEST_EXCLUDE_FROM_MAIN 1
)
ExternalProject_Add(simple-platformer
	DEPENDS
		Corrade
		Magnum
		MagnumPlugins
	SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src"
	LIST_SEPARATOR "#"
	CMAKE_ARGS
		"-DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}"
		"-DCMAKE_MODULE_PATH=${DEPS_INSTALL_DIR}#${CMAKE_CURRENT_BINARY_DIR}/Magnum-prefix/src/Magnum/modules"
		${CUSTOM_SIMPLEPLATFORMER_ARGS}
)
