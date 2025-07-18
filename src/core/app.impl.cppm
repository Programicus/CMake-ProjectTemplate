module;

#include "log/log.h"

#include <format>
#include <span>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

export module core.app:impl;
import core.app;

using namespace core;

void dump_env_to_log()
{
#ifdef _WIN32
	LPCH env_strings = GetEnvironmentStrings();

	for (LPCH var = env_strings; *var; var += strlen(var) + 1) {
		LOG_TRACE("Environment variable: {}", std::string_view(var));
	}

	FreeEnvironmentStrings(env_strings);
#else
	extern char** environ;
	for (char** env = environ; *env; ++env) {
		LOG_TRACE("Environment variable: {}", std::string_view(*env));
	}
#endif
}

app* app::s_instance = nullptr;

app::app(size_t argc, const char** argv)
	: m_args(argv, argc)
{
	ASSERT(s_instance == nullptr, "App already constructed");
	s_instance = this;
	core::log::init("logs", true);
	std::string arg_str;
	for (const char* arg : std::span(argv, argc))
	{
		arg_str += std::format(R"({} )", arg);
	}
	LOG_DEBUG("Command line arguments: \"{}\"", arg_str);
	dump_env_to_log();
}

app::~app()
{
	spdlog::shutdown();
	s_instance = nullptr;
}

app& app::GetApp()
{
	ASSERT(s_instance != nullptr, "App hasn't been constructed");
	return *s_instance;
}