cmake_minimum_required(VERSION 3.12)

project(<Name Of Project Here> VERSION 0.0.0 LANGUAGES CXX)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(setup/ProjectSetup.cmake)

add_subdirectory(external EXCLUDE_FROM_ALL) #all will not auto build these, but they will get imported as dependencies
add_subdirectory(src)

enable_testing()
add_subdirectory(test)