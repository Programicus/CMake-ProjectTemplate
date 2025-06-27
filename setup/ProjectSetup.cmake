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

# Create the project_warnings interface target
add_library(project_warnings INTERFACE)

# Detect clang-cl specifically
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
    set(CLANG_CL TRUE)
else()
    set(CLANG_CL FALSE)
endif()

# Clear all lists first
set(MSVC_OPTIONS "")
set(LINUX_OPTIONS "")
set(GCC_SPECIFIC_OPTIONS "")
set(CLANG_COMMON_OPTIONS "")
set(CLANG_SPECIFIC_OPTIONS "")

# MSVC options (used by MSVC and clang-cl)
list(APPEND MSVC_OPTIONS
    /W4                            # Enable warning level 4 (highest reasonable level)
    /WX                            # Treat warnings as errors

    # Enable additional warnings not included in /W4
    /w44242                        # Conversion from 'type1' to 'type2', possible loss of data
    /w44254                        # Conversion from 'type1' to 'type2', possible loss of data
    /w44263                        # Member function does not override any base class virtual member function
    /w44265                        # Class has virtual functions, but destructor is not virtual
    /w44287                        # Unsigned/negative constant mismatch
    /w44289                        # Nonstandard extension used: 'var' : loop control variable declared in the for-loop is used outside the for-loop scope
    /w44296                        # Expression is always false/true
    /w44311                        # Pointer truncation from 'type1' to 'type2'
    /w44342                        # Behavior change: 'function' called, but a member operator was called in previous versions
    /w44350                        # Behavior change: 'member1' called instead of 'member2'
    /w44355                        # 'this' : used in base member initializer list
    /w44365                        # Conversion from 'type1' to 'type2', signed/unsigned mismatch
    /w44370                        # Layout of class has changed from a previous version of the compiler due to better packing
    /w44371                        # Layout of class may have changed from a previous version of the compiler due to better packing of member
    /w44388                        # Signed/unsigned mismatch
    /w44412                        # Function signature contains type 'type' which is ABI-incompatible between C and C++
    /w44426                        # Optimization flags changed after including header
    /w44456                        # Declaration of 'identifier' hides previous local declaration
    /w44457                        # Declaration of 'identifier' hides function parameter
    /w44458                        # Declaration of 'identifier' hides class member
    /w44459                        # Declaration of 'identifier' hides global declaration
    /w44471                        # A forward declaration of an unscoped enumeration must have an underlying type
    # Removed /w44571 - catch(...) semantics warning (irrelevant without exceptions)
    /w44640                        # Local static object is not thread-safe
    /w44668                        # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
    /w44742                        # Variable has different alignment in different translation units
    /w44774                        # Format string expected in argument is not a string literal
    /w44777                        # Format string requires an argument of type 'type1', but variadic argument has type 'type2'
    /w44800                        # Implicit conversion from 'type' to bool, possible performance warning

    # Disabled warnings
    /wd4201                        # Nonstandard extension used: nameless struct/union (for anonymous unions)
    /wd4625                        # Copy constructor was implicitly defined as deleted
    /wd4624                        # Destructor was implicitly defined as deleted (for singletons)
    /wd4626                        # Assignment operator was implicitly defined as deleted
    /wd4627                        # assignment operator was implicitly defined as deleted 
    /wd5105                        # Macro expansion producing 'defined' has undefined behavior
    /wd5030                        # Attribute 'attribute' is not recognized (enabled in code generator)

    # Conformance options
    /Zc:preprocessor               # Use the new conforming preprocessor (for __VA_OPT__ support)
    /permissive-                   # Enable strict standards conformance
    /Zc:__cplusplus                # Enable updated __cplusplus macro value
    /Zc:inline                    # Remove unreferenced COMDAT
    # Removed /Zc:throwingNew - irrelevant without exceptions
)

# Linux options (common base for GCC and Clang on Linux)
list(APPEND LINUX_OPTIONS

    # Enable all warnings
    -fno-rtti                      #disable RTTI to link against llvm (not used by gaze)
    -fno-exceptions                #disable exceptions to link against llvm (not used by gaze)

    # Basic warning sets
    -Wall                          # Enable most common warnings
    -Wextra                        # Enable extra warning flags not enabled by -Wall
    -Werror                        # Treat warnings as errors

    # Variable shadowing and scoping
    -Wshadow                       # Variable declaration shadows one from a parent context

    # Class design warnings
    -Wnon-virtual-dtor             # Class with virtual functions has a non-virtual destructor (still relevant for inheritance)
    -Woverloaded-virtual           # Virtual function is overloaded (not overridden) in derived class

    # Type safety warnings
    -Wold-style-cast               # C-style casts in C++ code
    -Wcast-align                   # Cast increases required alignment of target type
    -Wconversion                   # Type conversions that may alter value
    -Wsign-conversion              # Implicit conversions between signed and unsigned integers

    # Usage warnings
    -Wunused                       # All -Wunused-* warnings

    # Code quality warnings
    -Wnull-dereference             # Null pointer dereference detected

    # Disabled warnings
    -Wno-injected-class-name       # Using injected class name as template argument
)

# GCC specific options (in addition to Linux options)
list(APPEND GCC_SPECIFIC_OPTIONS
    -Wmisleading-indentation       # Indentation implies blocks where blocks do not exist
    -Wduplicated-cond              # Duplicated conditions in if-else chain
    -Wduplicated-branches          # Duplicated code in if-else branches
    -Wlogical-op                   # Suspicious use of logical operators in expressions
    -Wuseless-cast                 # Cast to the same type
    -Wsuggest-override             # Virtual function could be marked override
    -Wstringop-truncation          # String operation truncation
    -Walloc-zero                   # Allocating zero bytes
    -Wformat-overflow=2            # Formatted output overflow checking
    -Wformat-truncation=2          # Formatted output truncation checking
    -Wstringop-overflow=4          # String operation overflow checking
    -Warray-bounds=2               # Out of bounds array access
    -Wshadow=local                 # Local variable shadows another local
    -Wshadow=compatible-local      # Local variable shadows compatible local
    -Wdelete-non-virtual-dtor      # Deleting object with non-virtual destructor
    -Wcast-qual                    # Cast discards qualifiers from pointer target type
    -Wfloat-conversion             # Implicit conversion turns floating-point number into integer
    -Wdouble-promotion             # Float is implicitly promoted to double
    -Wunused-parameter             # Unused function parameters
    -Wunused-const-variable=2      # Unused const variables at file scope
    -Wformat=2                     # Printf/scanf format string checking
    -Wimplicit-fallthrough         # Fallthrough in switch statement
    -Wmissing-field-initializers   # Missing fields in struct initializer
    -Wuninitialized                # Uninitialized variables
    -Wundef                        # Undefined identifier in #if directive
    -Wredundant-decls              # Multiple declarations of the same entity

    -Wno-changes-meaning           # Changes meaning of a macro
    -Wno-pedantic-main             # Allow taking address of main function
    -Wno-gnu-anonymous-struct      # Allows anonymous structs
)

# Clang common options (used by both clang and clang-cl)
list(APPEND CLANG_COMMON_OPTIONS
    # Type safety and conversion warnings
    -Wcast-qual                    # Cast discards qualifiers from pointer target type
    -Wfloat-conversion             # Implicit conversion turns floating-point number into integer
    -Wdouble-promotion             # Float is implicitly promoted to double
    -Wshorten-64-to-32             # 64-bit to 32-bit truncation
    -Wstring-conversion            # String literal to bool conversion

    # Usage warnings
    -Wunused-parameter             # Unused function parameters
    -Wunused-lambda-capture        # Unused lambda captures
    -Wunused-local-typedef         # Unused local typedefs

    # Code quality warnings
    -Wformat=2                     # Printf/scanf format string checking
    -Wformat-security              # Format string security issues
    -Wimplicit-fallthrough         # Fallthrough in switch statement
    -Wmissing-field-initializers   # Missing fields in struct initializer
    -Wuninitialized                # Uninitialized variables
    -Wconditional-uninitialized    # Conditionally uninitialized variables
    -Wundef                        # Undefined identifier in #if directive
    -Wredundant-decls              # Multiple declarations of the same entity

    # Shadow warnings
    -Wshadow-field                 # Field shadowing in C++
    -Wshadow-uncaptured-local      # Shadowing of uncaptured locals

    # Analysis warnings
    -Wthread-safety                # Thread safety annotations
    -Wunreachable-code             # Unreachable code
    -Wunreachable-code-break       # Unreachable break statements
    -Wunreachable-code-return      # Unreachable return statements
    -Wcomma                        # Suspicious uses of comma operator
    -Wloop-analysis                # Suspicious loop constructs
    -Wself-assign                  # Self assignment
    -Wself-move                    # Self move
    -Wmove                         # Pessimizing and redundant moves
    -Wrange-loop-analysis          # Range-based for loop issues
    -Watomic-implicit-seq-cst      # Implicit seq_cst atomic operations
    -Wdelete-non-virtual-dtor      # Deleting object with non-virtual destructor
    
    # Disabled warnings due to gaze's api
    -Wno-unknown-attributes
    -Wno-undefined-var-template
    -Wno-missing-braces
)

# Clang specific options (only for regular clang, not clang-cl)
list(APPEND CLANG_SPECIFIC_OPTIONS
    -Wno-c++98-compat              # Don't warn about C++98 incompatibilities
    -Wno-c++98-compat-pedantic     # Don't warn about pedantic C++98 incompatibilities
    -Wno-nested-anon-types         # Allow nested anonymous types (common pattern)
    -Wno-newline-eof               # Don't require newline at end of file
    -Wno-extra-semi-stmt           # Allow extra semicolons
    -Wno-nonportable-system-include-path  # Don't warn about case-sensitive includes
    -Wno-float-equal               # Allow float equality comparisons
    -Wno-gnu-zero-variadic-macro-arguments  # Allow zero variadic macro arguments
    -Wno-range-loop-bind-reference # Don't warn about binding reference in range-for
    -Wno-ctad-maybe-unsupported    # Don't warn about class template argument deduction
    -Wno-unused-macros             # Don't warn about unused macros
)

# Apply warning options based on compiler
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND NOT CLANG_CL)
    # Pure MSVC
    target_compile_options(project_warnings
        INTERFACE
        ${MSVC_OPTIONS}
    )
elseif(CLANG_CL)
    # clang-cl: MSVC options + Clang common options with /clang: prefix
    set(CLANG_CL_OPTIONS ${MSVC_OPTIONS})

    # Add clang common options with /clang: prefix
    foreach(option ${CLANG_COMMON_OPTIONS})
        list(APPEND CLANG_CL_OPTIONS "/clang:${option}")
    endforeach()

    # Add clang-cl specific option
    list(APPEND CLANG_CL_OPTIONS "-Qunused-arguments")

    target_compile_options(project_warnings
        INTERFACE
        ${CLANG_CL_OPTIONS}
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC: Linux options + GCC specific
    target_compile_options(project_warnings
        INTERFACE
        ${LINUX_OPTIONS}
        ${GCC_SPECIFIC_OPTIONS}
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang: Linux options + Clang common + Clang specific
    target_compile_options(project_warnings
        INTERFACE
        ${LINUX_OPTIONS}
        ${CLANG_COMMON_OPTIONS}
        ${CLANG_SPECIFIC_OPTIONS}
    )
endif()
