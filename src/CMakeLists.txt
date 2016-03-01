cmake_minimum_required(VERSION 3.3)

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

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/config.hh"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

corrade_add_resource(SimplePlatformer_Resources "data.corrade")

option(SHOW_WINDOWS_CONSOLE "Show the console on Windows" $<$<CONFIG:Debug>:ON>)

if(WIN32 AND NOT SHOW_WINDOWS_CONSOLE)
	add_executable(simple-platformer WIN32
		"Main.cpp"
	)
else()
	add_executable(simple-platformer
		"Main.cpp"
	)
endif()
target_sources(simple-platformer PRIVATE
	${SimplePlatformer_Resources}
)
set_property(TARGET simple-platformer PROPERTY CXX_STANDARD 14)
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
)

install(
	TARGETS
		simple-platformer
	RUNTIME DESTINATION bin
)

enable_testing()

#...
