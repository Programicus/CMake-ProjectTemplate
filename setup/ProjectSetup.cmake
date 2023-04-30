add_library(project_warnings INTERFACE) #set as an interface so as to not apply to external dependencies

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(project_warnings
		INTERFACE

		/W4
		/WX								# turn warnings into errors
		/wd4624							# disable warning of implictely deleted dtor, to allow for singletons
		/wd5105							# disable warning about "defined" in macro expansion
		/Zc:preprocessor				# standard conformit preprocessor (namely for __VA_OPT__)
		/permissive-					# strict standard conformance
	)
else()
	target_compile_options(project_warnings
		INTERFACE

		-Wall
		-Wextra
		-Werror
		-Wshadow                       	# warn the user if a variable declaration shadows one from a parent context
		-Wnon-virtual-dtor             	# warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
		-Wold-style-cast 				# warn for c-style casts
		-Wcast-align 					# warn for potential performance problem casts
		-Wunused 						# warn on anything being unused
		-Woverloaded-virtual 			# warn if you overload (not override) a virtual function
		-Wpedantic 						# warn if non-standard C++ is used
		-Wconversion 					# warn on type conversions that may lose data
		-Wsign-conversion 				# warn on sign conversions   
		-Wnull-dereference 				# warn if a null dereference is detected
		-Wno-injected-class-name
	)
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" ) #these warnings are gcc only, clang doesn't support them
		target_compile_options(project_warnings
			INTERFACE

			-Wmisleading-indentation 	# warn if identation implies blocks where blocks do not exist
			-Wduplicated-cond 			# warn if if / else chain has duplicated conditions
			-Wduplicated-branches 		# warn if if / else branches have duplicated code
			-Wlogical-op 				# warn about logical operations being used where bitwise were probably wanted
			-Wuseless-cast 				# warn if you perform a cast to the same type
		)
	endif()
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_compile_options(project_warnings
			INTERFACE
			-Wno-c++98-compat
			-Wno-c++98-compat-pedantic
			-Wno-nested-anon-types
			-Wno-newline-eof
			-Wno-extra-semi-stmt
			-Wno-nonportable-system-include-path
			-Wno-float-equal
			-Wno-gnu-zero-variadic-macro-arguments
			-Wno-range-loop-bind-reference
			-Wno-ctad-maybe-unsupported
			-Wno-unused-macros
		)
	endif()
endif()

function(debug_proj name)
	message("======================${name}======================")
	get_target_property(interface_includes ${name} INTERFACE_INCLUDE_DIRECTORIES)
	get_target_property(includes ${name} INCLUDE_DIRECTORIES)
	get_target_property(libs ${name} LINK_INTERFACE_LIBRARIES)
	get_target_property(interface_libs ${name} LINKLIBRARIES)
	message("include dirs: ${interface_includes} : ${includes}")
	message("include dirs: ${interface_libs} : ${libs}")
	message("======================${name}======================")
endfunction()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)