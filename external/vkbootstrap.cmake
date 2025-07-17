include(FetchContent)

FetchContent_Declare(
    vk-bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG "v${Vulkan_VERSION}"
)

set(VK_BOOTSTRAP_WERROR ON CACHE BOOL "" FORCE)
set(VK_BOOTSTRAP_TEST OFF CACHE BOOL "" FORCE)
set(VK_BOOTSTRAP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(vk-bootstrap)

set_target_properties(vk-bootstrap PROPERTIES
    FOLDER "External/vk-bootstrap"
)

