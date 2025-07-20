add_subdirectory(log)
add_subdirectory(assert)

add_library(core STATIC
	app.cpp
)

target_sources(core PUBLIC
	FILE_SET CXX_MODULES FILES
	app.cppm
	core.cppm
	scope_guard.cppm
)

target_link_libraries(core PUBLIC
	project_warnings
	spdlog
	assert
	log
)

target_compile_definitions(spdlog INTERFACE SPDLOG_ACTIVE_LEVEL=0)
target_compile_definitions(spdlog INTERFACE SPDLOG_NO_EXCEPTIONS)

