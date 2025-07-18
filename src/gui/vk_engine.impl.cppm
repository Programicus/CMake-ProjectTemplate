module;


#include "vk_types.h"
#include "vkbootstrap_wrapper.h"


#include <glfw/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <chrono>
#include <vector>


export module gui.vk.engine:impl;

import gui.vk.engine;
import gui.vk.init;

void VulkanEngine::init(const char* windowName)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	window = glfwCreateWindow((int)windowExtent.width, (int)windowExtent.height, windowName, nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);

	init_vulkan(windowName);
	init_swapchain();
	init_commands();
	init_default_renderpass();
	init_framebuffers();
	init_sync_structures();
	init_descriptors();

	init_imgui();

	isInitialized = true;
}

void VulkanEngine::cleanup()
{
	if (isInitialized)
	{

		auto res = device.waitForFences(renderFence, true, static_cast<uint64_t>(waitTimeout.count()));
		ASSERT(res == vk::Result::eSuccess);
		device.resetFences(renderFence);

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		device.destroyDescriptorPool(descriptorPool);

		device.destroySemaphore(presentSemaphore);
		device.destroySemaphore(renderSemaphore);
		device.destroyFence(renderFence);

		device.destroyCommandPool(commandPool);

		teardown_swapchain();

		device.destroy();
		instance.destroySurfaceKHR(surface);
		vkb::destroy_debug_utils_messenger(instance, debugMessenger);
		instance.destroy();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
}
ImVec4 dodgerBlue = ImVec4(30.f / 255.f, 144.f / 255.f, 1.0f, 1.0f);

void VulkanEngine::new_frame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoDocking;

	auto* viewPort = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewPort->Pos);
	ImGui::SetNextWindowSize(viewPort->Size);
	ImGui::SetNextWindowViewport(viewPort->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{});

	ImGui::Begin("Merlin", nullptr, windowFlags);

	ImGui::PopStyleVar(2);

}
void VulkanEngine::render()
{
	ImGui::End();
	if (is_minimized())
	{
		recreate_swapchain();
		return;
	}
	ImGui::Render();

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	if (is_minimized())
	{
		recreate_swapchain();
		return;
	}

	auto res = device.waitForFences(renderFence, true, static_cast<uint64_t>(waitTimeout.count()));
	ASSERT(res == vk::Result::eSuccess);
	device.resetFences(renderFence);
	auto aquireRes = device.acquireNextImageKHR(swapchain, static_cast<uint64_t>(waitTimeout.count()), presentSemaphore, VK_NULL_HANDLE);
	res = aquireRes.result;
	if (res == vk::Result::eSuboptimalKHR || res == vk::Result::eErrorOutOfDateKHR)
	{
		recreate_swapchain();
		return;
	}
	ASSERT(res == vk::Result::eSuccess);
	uint32_t fbIdx = aquireRes.value;

	mainCommandBuffer.reset();

	vk::CommandBuffer& cmd = mainCommandBuffer;
	vk::CommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	auto beginRes = cmd.begin(cmdBeginInfo);
	ASSERT(beginRes == vk::Result::eSuccess);

	vk::ClearValue clearValue;
	clearValue.color.float32[0] = dodgerBlue.x;
	clearValue.color.float32[1] = dodgerBlue.y;
	clearValue.color.float32[2] = dodgerBlue.z;
	clearValue.color.float32[3] = dodgerBlue.w;

	vk::RenderPassBeginInfo info;
	info.renderPass = renderPass;
	info.renderArea.offset.x = 0;
	info.renderArea.offset.y = 0;
	info.renderArea.extent = windowExtent;
	info.renderArea.extent = windowExtent;
	info.framebuffer = framebuffers[fbIdx];

	info.setClearValues(clearValue);
	cmd.beginRenderPass(info, vk::SubpassContents::eInline);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	cmd.endRenderPass();
	res = cmd.end();

	vk::SubmitInfo submit;
	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submit.pWaitDstStageMask = &waitStage;
	submit.setWaitSemaphores(presentSemaphore);
	submit.setSignalSemaphores(renderSemaphore);
	submit.setCommandBuffers(cmd);

	res = graphicsQueue.submit(submit, renderFence);

	vk::PresentInfoKHR presentInfo;
	presentInfo.setSwapchains(swapchain);
	presentInfo.setWaitSemaphores(renderSemaphore);
	presentInfo.setImageIndices(fbIdx);

	res = graphicsQueue.presentKHR(&presentInfo);
	if (res == vk::Result::eSuboptimalKHR || res == vk::Result::eErrorOutOfDateKHR)
	{
		recreate_swapchain();
		return;
	}
	ASSERT(res == vk::Result::eSuccess);

	frameNumber++;
}

void VulkanEngine::init_vulkan(const char* windowName)
{
	vkb::InstanceBuilder builder;
	auto inst_ret = builder.set_app_name(windowName)
		.set_engine_name("Vulkan Engine")
		.request_validation_layers(true)
		.require_api_version(1, 1, 0)
		.use_default_debug_messenger()
		.build();
	vkb::Instance vkb_inst = inst_ret.value();
	instance = vkb_inst.instance;
	debugMessenger = vkb_inst.debug_messenger;

	// Get the surface
	VkSurfaceKHR csurface;
	glfwCreateWindowSurface(instance, window, nullptr, &csurface);
	this->surface = csurface;

	// Select a physical device
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1)
		.set_surface(surface)
		.select()
		.value();

	// Create the logical device
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();
	device = vkbDevice.device;
	chosenGPU = physicalDevice.physical_device;

	graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanEngine::teardown_swapchain()
{
	auto res = device.waitIdle();
	ASSERT(res == vk::Result::eSuccess);
	for (auto framebuffer : framebuffers)
		device.destroyFramebuffer(framebuffer);
	framebuffers.clear();
	device.destroyRenderPass(renderPass);
	for (auto imageView : swapchainImageViews)
		device.destroyImageView(imageView);
	swapchainImageViews.clear();
	device.destroySwapchainKHR(swapchain);
}

void VulkanEngine::init_swapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{ chosenGPU, device, surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.add_fallback_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR })
		//.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		//.set_desired_extent(windowExtent.width, windowExtent.height)
		.build()
		.value();

	swapchain = vkbSwapchain.swapchain;
	const auto vkImages = vkbSwapchain.get_images().value();
	swapchainImages.reserve(vkImages.size());
	for (auto& vkImage : vkImages)
		swapchainImages.emplace_back(vkImage);

	const auto vkImageViews = vkbSwapchain.get_image_views().value();
	swapchainImageViews.reserve(vkImageViews.size());
	for (auto& vkImageView : vkImageViews)
		swapchainImageViews.emplace_back(vkImageView);

	swapchainImageFormat = (vk::Format)vkbSwapchain.image_format;
}

void VulkanEngine::init_commands()
{
	vk::CommandPoolCreateInfo commandPoolInfo = renderer::command_pool_create_info(graphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	commandPool = device.createCommandPool(commandPoolInfo).value;

	vk::CommandBufferAllocateInfo cmdAllocInfo = renderer::command_buffer_allocate_info(commandPool, 1);
	mainCommandBuffer = device.allocateCommandBuffers(cmdAllocInfo).value[0];
}

void VulkanEngine::init_default_renderpass()
{
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.setColorAttachments(colorAttachmentRef);

	vk::SubpassDependency subpassDependency = {};
	subpassDependency.srcSubpass = vk::SubpassExternal;
	subpassDependency.dstSubpass = {};
	subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	subpassDependency.srcAccessMask = {};
	subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

	vk::RenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.setAttachments(colorAttachment);
	renderPassInfo.setSubpasses(subpass);
	renderPassInfo.setDependencies(subpassDependency);

	auto res = device.createRenderPass(&renderPassInfo, nullptr, &renderPass);
	(void)res;
}

void VulkanEngine::init_framebuffers()
{
	vk::FramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.setRenderPass(renderPass);
	framebufferInfo.setWidth(windowExtent.width);
	framebufferInfo.setHeight(windowExtent.height);
	framebufferInfo.setLayers(1);
	framebuffers.resize(swapchainImageViews.size());
	for (size_t i = 0; i < swapchainImageViews.size(); i++)
	{
		framebufferInfo.setAttachments(swapchainImageViews[i]);
		framebuffers[i] = device.createFramebuffer(framebufferInfo).value;
	}
}

void VulkanEngine::init_sync_structures()
{
	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	renderFence = device.createFence(fenceInfo).value;
	vk::SemaphoreCreateInfo semaphoreInfo = {};
	presentSemaphore = device.createSemaphore(semaphoreInfo).value;
	renderSemaphore = device.createSemaphore(semaphoreInfo).value;
}

void VulkanEngine::init_descriptors()
{
	vk::DescriptorPoolSize poolSizes[] =
	{
	{ vk::DescriptorType::eCombinedImageSampler, 1 }
	};

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.setPoolSizes(poolSizes);
	poolInfo.setMaxSets(1);
	poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
	descriptorPool = device.createDescriptorPool(poolInfo).value;
}

void VulkanEngine::init_imgui()
{
	ImGui::CreateContext();

	auto& io = ImGui::GetIO();
	io.DisplaySize.x = (float)windowExtent.width;
	io.DisplaySize.y = (float)windowExtent.height;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForVulkan(window, true);

	init_imgui_vulkan();
}

void VulkanEngine::init_imgui_vulkan()
{
	ImGui_ImplVulkan_InitInfo init_info;
	memset(&init_info, 0, sizeof(init_info));
	init_info.Instance = instance;
	init_info.PhysicalDevice = chosenGPU;
	init_info.Device = device;
	init_info.QueueFamily = graphicsQueueFamily;
	init_info.Queue = graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = descriptorPool;
	init_info.Subpass = 0;
	init_info.MinImageCount = (uint32_t)swapchainImages.size();
	init_info.ImageCount = (uint32_t)swapchainImages.size();
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = VK_NULL_HANDLE;
	init_info.RenderPass = renderPass;
	ImGui_ImplVulkan_Init(&init_info);
}

void VulkanEngine::recreate_swapchain()
{
	glfwGetFramebufferSize(window, (int*)&windowExtent.width, (int*)&windowExtent.height);
	if (is_minimized())
		return;

	auto res = device.waitIdle();
	ASSERT(res == vk::Result::eSuccess);
	teardown_swapchain();
	init_swapchain();
	init_default_renderpass();
	init_framebuffers();
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = (float)windowExtent.width;
	io.DisplaySize.y = (float)windowExtent.height;
}

bool VulkanEngine::is_minimized()
{
	return windowExtent.width == 0 || windowExtent.height == 0;
}