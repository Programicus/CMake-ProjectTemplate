module;


#include "vk_types.h"

#include <chrono>

export module gui.vk.engine;
import gui.vk.init;

using namespace std::chrono_literals;
export class VulkanEngine
{
public:
	bool isInitialized = false;
	int frameNumber = 0;

	vk::Extent2D windowExtent{ 800, 600 };//{ 1920, 1080 };

	struct GLFWwindow* window = nullptr;

	// Vulkan Components
	void init(const char*);
	void cleanup();
	void new_frame();
	void render();

private:
	constexpr static std::chrono::nanoseconds waitTimeout = 1s;

	vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugMessenger;
	vk::PhysicalDevice chosenGPU;
	vk::Device device;
	vk::SurfaceKHR surface;

	vk::SwapchainKHR swapchain;
	vk::Format swapchainImageFormat;
	std::vector<vk::Image> swapchainImages;
	std::vector<vk::ImageView> swapchainImageViews;

	vk::Queue graphicsQueue;
	uint32_t graphicsQueueFamily;
	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;
	vk::RenderPass renderPass;
	std::vector<vk::Framebuffer> framebuffers;

	vk::Semaphore presentSemaphore;
	vk::Semaphore renderSemaphore;
	vk::Fence renderFence;

	vk::DescriptorPool descriptorPool;

	void init_vulkan(const char*);
	void init_swapchain();
	void teardown_swapchain();
	void init_commands();
	void init_default_renderpass();
	void init_framebuffers();
	void init_sync_structures();
	void init_descriptors();

	void init_imgui();
	void init_imgui_vulkan();

	void recreate_swapchain();

	bool is_minimized();
};