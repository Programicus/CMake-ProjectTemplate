add_library(assert STATIC)

target_sources(assert
	PUBLIC
	assert.h
)

target_sources(assert
	PRIVATE
	assert.cpp
)

target_precompile_headers(assert PUBLIC assert.h)
target_include_directories(assert
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(assert PUBLIC
	log
)

if(WIN32)
	target_compile_options(log PUBLIC 
		/Zc:preprocessor					# Use the new conforming preprocessor (for __VA_OPT__ support)
		/permissive-						# Enable strict standards conformance
		/Zc:__cplusplus						# Enable updated __cplusplus macro value
		/Zc:inline							# Remove unreferenced COMDAT
	)
endif()