find_package(Vulkan GLOBAL REQUIRED)
target_compile_definitions(Vulkan::Vulkan INTERFACE VULKAN_HPP_NO_EXCEPTIONS)

message(STATUS "Vulkan version: ${Vulkan_VERSION}")

include(spdlog.cmake)
include(glfw.cmake)
include(imgui.cmake)
include(vkbootstrap.cmake)