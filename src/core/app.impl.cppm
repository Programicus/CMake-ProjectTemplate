module;

#include "log/log.h"

export module core.app:impl;
import core.app;

using namespace core;

app* app::s_instance = nullptr;

app::app(size_t argc, const char** argv)
	: m_args(argv, argc)
{
	ASSERT(s_instance == nullptr, "App already constructed");
	s_instance = this;
	core::log::init("logs", true);
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