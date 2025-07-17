#include "log.h"

#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/null_sink.h>
#ifdef _WIN32
#include <spdlog/sinks/msvc_sink.h>
#endif

#include <chrono>

[[maybe_unused]] const auto folder_sep_filename = spdlog::details::os::folder_seps_filename;
[[maybe_unused]] const auto folder_sep = spdlog::details::os::folder_seps;

class uptime_seconds_flag : public spdlog::custom_flag_formatter
{
public:
	void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override
	{
		static auto start_time = std::chrono::steady_clock::now();
		auto now = std::chrono::steady_clock::now();
		auto seconds = std::chrono::duration<double>(now - start_time).count();

		fmt::format_to(std::back_inserter(dest), "{:>8.3f}", seconds);
	}

	std::unique_ptr<custom_flag_formatter> clone() const override
	{
		return spdlog::details::make_unique<uptime_seconds_flag>();
	}
};

void core::log::init(std::string_view log_path, bool log_to_stdout) noexcept
{
	spdlog::cfg::load_env_levels("GAZE_LOG_LEVEL");

	auto logger = spdlog::default_logger();

	// Clear all existing sinks
	logger->sinks().clear();

	logger->set_level(spdlog::level::trace);

	// Add your sinks
	if (!log_path.empty()) {
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
			std::string(log_path), true
		);
		auto formatter = std::make_unique<spdlog::pattern_formatter>();
		formatter->add_flag<uptime_seconds_flag>('U');  // %U for uptime
		formatter->set_pattern("[%U] [%l] [%t] %v");
		file_sink->set_formatter(std::move(formatter));
		logger->sinks().push_back(file_sink);
	}

	if (log_to_stdout) {
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto formatter = std::make_unique<spdlog::pattern_formatter>();
		formatter->add_flag<uptime_seconds_flag>('U');
		formatter->set_pattern("[%U] [%^%l%$] %v");
		console_sink->set_formatter(std::move(formatter));
		logger->sinks().push_back(console_sink);

	}

#ifdef _WIN32
	auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	msvc_sink->set_pattern("[%l] %v");  // Keep it simple for debug output
	logger->sinks().push_back(msvc_sink);
#endif

	// Ensure we have at least one sink
	if (logger->sinks().empty()) {
		// Add null sink or default stdout
		auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
		logger->sinks().push_back(null_sink);
	}
}