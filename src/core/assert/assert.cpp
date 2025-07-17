#include "assert.h"

#include <log.h>

#include <spdlog/fmt/fmt.h>

#include <utility>

using namespace core::assert;

handled default_assert_handler(const assert_info& info, void* /*data*/) noexcept
{

	auto formatted_message = fmt::vformat(info.message, info.fmt_args);

	LOG_CRITICAL(R"(Assertion failed: "{}", 

{}({})

Failing expression: {}
Formatted message: {}
Unformatted message: {}
Function: {}
File: {}
Line: {}
Column: {}



)",
formatted_message,
info.loc.file_name(), info.loc.line(),
info.expr,
formatted_message,
info.message,
info.loc.function_name(),
info.loc.file_name(),
info.loc.line(),
info.loc.column()
);

	return handled::kNo;
}

namespace
{
	assert_handler_t current_handler = default_assert_handler;
	void* current_handler_data = nullptr;
}

assert_handler_t core::assert::set_assert_handler(assert_handler_t new_handler) noexcept
{
	return std::exchange(current_handler, new_handler);
}

void* core::assert::set_assert_handler_data(void* new_data) noexcept
{
	return std::exchange(current_handler_data, new_data);
}

handled core::assert::on_assert(const assert_info& info) noexcept
{
	return current_handler(info, current_handler_data);
}