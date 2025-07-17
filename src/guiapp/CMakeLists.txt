add_library(gui STATIC
	imgui_ext.h
	vk_types.h
	context.cpp
	context.welcome.cpp
)

target_sources(gui PUBLIC
	FILE_SET CXX_MODULES FILES
	app.cppm
	component.cppm
	context.cppm
	context.welcome.cppm
	gui.cppm
	
	vk_engine.cppm
	vk_engine.impl.cppm
	vk_init.cppm
)

target_link_libraries(gui PUBLIC project_warnings
	core
	imgui
	Vulkan::Vulkan
	vk-bootstrap
)