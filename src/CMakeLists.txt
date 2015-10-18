cmake_minimum_required(VERSION 3.3)

project(simple-platformer)

find_package(Magnum REQUIRED
	MeshTools
	Primitives
	Shaders
	Sdl2Application
)

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
set_property(TARGET simple-platformer PROPERTY CXX_STANDARD 14)
target_include_directories(simple-platformer
	PUBLIC
		${MAGNUM_INCLUDE_DIRS}
		${MAGNUM_APPLICATION_INCLUDE_DIRS}
)
target_link_libraries(simple-platformer
	${MAGNUM_APPLICATION_LIBRARIES}
	${MAGNUM_LIBRARIES}
	${MAGNUM_MESHTOOLS_LIBRARIES}
	${MAGNUM_PRIMITIVES_LIBRARIES}
	${MAGNUM_SHADERS_LIBRARIES}
)
if(WIN32 AND MAGNUM_BUILD_STATIC)
	# Link SDL dependencies
	target_link_libraries(simple-platformer
		mingw32
		m
		dinput8
		dxguid
		dxerr8
		user32
		gdi32
		winmm
		imm32
		ole32
		oleaut32
		shell32
		version
		uuid
	)
endif()
