#pragma once

#include <source_location>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/args.h>

#if defined(_MSC_VER)
#define BREAKPOINT_IMPL() __debugbreak()
#elif defined(__X86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86) || defined(__amd64__)
#define BREAKPOINT_IMPL() __asm__ __volatile__("int $3")
#elif defined(__aarch64__)
#define BREAKPOINT_IMPL() __builtin_debugtrap()
#else
#error "Unsupported platform"
#endif

#define breakpoint BREAKPOINT_IMPL()

namespace core::assert
{
	inline constexpr bool enabled = true;
	inline constexpr size_t max_message_length = 1024;

	struct assert_info
	{
		const char* expr;
		const char* message;
		fmt::dynamic_format_arg_store<fmt::format_context> fmt_args;
		std::source_location loc;

		assert_info(std::source_location loc, const char* expr)
			: expr(expr), message(expr), fmt_args({}), loc(loc)
		{
		}
		assert_info(std::source_location loc, const char* expr, const char* msg)
			: expr(expr), message(msg), fmt_args({}), loc(loc)
		{
		}
		template<typename... Args>
		assert_info(std::source_location loc, const char* expr, const char* msg, Args&&... args)
			: expr(expr), message(msg), fmt_args(), loc(loc)
		{
			(fmt_args.push_back(std::forward<Args>(args)), ...);
		}
	};

	enum class handled
	{
		kNo,
		kYes
	};

	handled on_assert(const assert_info& info) noexcept;

	using assert_handler_t = handled(*)(const assert_info&, void* data) noexcept;

	assert_handler_t set_assert_handler(assert_handler_t new_handler) noexcept;
	void* set_assert_handler_data(void* new_data) noexcept;

}


#define ASSERT(expr,...)																			\
do{																									\
	if constexpr (core::assert::enabled)															\
	{																								\
		if (!static_cast<bool>(expr))																\
		{																							\
																									\
			/* in the constexpr case just abort, forces compiler warning*/							\
			[](){ if (std::is_constant_evaluated()) std::abort();}();								\
																									\
			core::assert::assert_info __info														\
			(																						\
				std::source_location::current(), #expr __VA_OPT__(,) __VA_ARGS__					\
			);																						\
																									\
			const auto was_handled = core::assert::on_assert(__info);								\
																									\
			if(was_handled == core::assert::handled::kNo)											\
			{																						\
				breakpoint;																			\
			}																						\
		}																							\
	}																								\
} while (false)

#define IM_ASSERT(expr) ASSERT(expr, "IMGUI Assert: {}", #expr)
#define VULKAN_HPP_ASSERT(expr) ASSERT(expr, "Vulkan-Hpp Assert: {}", #expr)