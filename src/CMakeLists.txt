cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "Must be at least 14")
option(CMAKE_CXX_EXTENSIONS "C++ language extensions" OFF)

project(simple-platformer
	VERSION 0.0.0
	LANGUAGES CXX
)

find_package(Corrade REQUIRED)
find_package(Magnum REQUIRED
	COMPONENTS
		MeshTools
		Primitives
		SceneGraph
		Shaders
		Sdl2Application
		Text
)
find_package(MagnumExtras REQUIRED
	COMPONENTS
		Ui
)
find_library(SDL2_MAIN
	NAMES
		SDL2main
)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/config.in.h"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

corrade_add_resource(SimplePlatformer_Resources "data.corrade")

add_executable(simple-platformer
	"Main.cpp"
	${SimplePlatformer_Resources}
)
set_target_properties(simple-platformer
	PROPERTIES
		WIN32_EXECUTABLE $<CONFIG:Release>
)
target_include_directories(simple-platformer
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(simple-platformer
	Magnum::Magnum
	Magnum::Application
	Magnum::MeshTools
	Magnum::Primitives
	Magnum::SceneGraph
	Magnum::Shaders
	Magnum::Text
	MagnumExtras::Ui
	$<$<CONFIG:Release>:${SDL2_MAIN}>
)

install(
	TARGETS
		simple-platformer
	RUNTIME DESTINATION bin
)

add_subdirectory(object)
add_subdirectory(ui)

enable_testing()

#...
