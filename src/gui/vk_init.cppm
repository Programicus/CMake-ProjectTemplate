module;

#include "vk_types.h"

export module gui.vk.init;


export namespace renderer
{
	vk::CommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags flags)
	{
		vk::CommandPoolCreateInfo info{};
		info.queueFamilyIndex = queueFamilyIndex;
		info.flags = flags;
		return info;
	}
	vk::CommandBufferAllocateInfo command_buffer_allocate_info(vk::CommandPool pool, uint32_t count = 1, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary)
	{
		vk::CommandBufferAllocateInfo info{};
		info.commandPool = pool;
		info.commandBufferCount = count;
		info.level = level;
		return info;
	}
}