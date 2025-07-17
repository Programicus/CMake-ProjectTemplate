add_library(log STATIC)

target_sources(log
	PRIVATE
	log.cpp
)
target_sources(log
	PUBLIC
	log.h
)

target_precompile_headers(log PUBLIC log.h)
target_include_directories(log
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(log PUBLIC
	spdlog
)