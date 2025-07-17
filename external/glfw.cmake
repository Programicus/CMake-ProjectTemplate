include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_LIBRARY_TYPE CACHE STRING "static" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)


FetchContent_MakeAvailable(glfw)

set_target_properties(glfw PROPERTIES
    FOLDER "External/glfw"
)