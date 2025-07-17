include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.1-docking
    SYSTEM
)

FetchContent_MakeAvailable(imgui)

add_library(imgui

    "${imgui_SOURCE_DIR}/imgui.h"
    "${imgui_SOURCE_DIR}/imgui_internal.h"
    "${imgui_SOURCE_DIR}/imstb_rectpack.h"
    "${imgui_SOURCE_DIR}/imstb_textedit.h"
    "${imgui_SOURCE_DIR}/imstb_truetype.h"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"

    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"

    "${imgui_SOURCE_DIR}/backends/vulkan/glsl_shader.frag"
    "${imgui_SOURCE_DIR}/backends/vulkan/glsl_shader.vert"

    "${imgui_SOURCE_DIR}/misc/debuggers/imgui.natvis"
    "${imgui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter"

    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)

target_include_directories(imgui
    PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends/"
)

target_link_libraries(imgui
    PUBLIC
    glfw
    Vulkan::Vulkan
    assert
)

set_target_properties(imgui PROPERTIES
    FOLDER "External/imgui"
)