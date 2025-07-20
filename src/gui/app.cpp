module;

#include "imgui_ext.h"

#include <imgui.h>
#include <glfw/glfw3.h>

#include <memory>
#include <filesystem>
#include <stack>
#include <ranges>
#include <vector>

module gui.app;

import core;
import gui.context;
import gui.context.welcome;
import gui.vk.engine;

namespace fs = std::filesystem;

using namespace gui;

app::app(size_t argc, const char** argv, const char* appName)
	:core::app(argc, argv)
{
	std::string temp;
	if (appName == nullptr)
	{
		const char* arg0 = argv[0];
		fs::path path{ arg0 };
		temp = path.filename().string();
		appName = temp.c_str();
	}
	m_engine.init(appName);
	emplace_context<welcome_ctx>();
}

int app::run()
{
	glfwShowWindow(m_engine.window);
	while (!glfwWindowShouldClose(m_engine.window))
	{
		glfwWaitEventsTimeout(1.0 / 60.);
		m_engine.new_frame();
		loop();
		m_engine.render();
	}
	glfwHideWindow(m_engine.window);
	return 0;
}

void app::request_exit()
{
	glfwSetWindowShouldClose(m_engine.window, GLFW_TRUE);
}

void app::focus(context* ctx)
{
	m_to_focus = ctx;
}

void app::loop()
{
	begin_menu_bar();
	ImGui::BeginTabBar("contexts");
	bool first = true;
	std::stack<size_t> to_close;
	auto ctxCopy = m_contexts	| std::ranges::views::transform([](const context_ptr& c) { return c.get(); })
								| std::ranges::to<std::vector>();

	context* to_focus = std::exchange(m_to_focus, nullptr);

	for (auto [idx, ctx] : ctxCopy | std::ranges::views::enumerate)
	{
		ImGuiExt::IdPusher id(ctx);

		auto name = ctx->name();

		bool* closeable = first ? nullptr : &(ctx->m_visible);
		ImGuiTabBarFlags flags = 0;
		if (first)
			flags |= ImGuiTabItemFlags_Leading;

		if (ctx == to_focus)
			flags |= ImGuiTabItemFlags_SetSelected;

		if (ctx->is_dirty())
			flags |= ImGuiTabItemFlags_UnsavedDocument;

		first = false;

		bool tab_visible = ImGui::BeginTabItem(name.data(), closeable, flags);

		core::scope_guard tab_closer([tab_visible]() {if (tab_visible) ImGui::EndTabItem(); });

		if (closeable && !*closeable)
		{
			if (ctx->about_to_close())
			{
				to_close.push(static_cast<size_t>(idx));
				LOG_TRACE("About to remove ctx {}, idx {}", name, idx);
				continue;
			}
			else
			{
				LOG_INFO("Not closing ctx {}, idx {} due to 'about_to_close' returning false", name, idx);
			}
		}

		ctx->loop(tab_visible);

		if (tab_visible)
			ctx->render();
	}

	ImGui::EndTabBar();

	update();

	if (!to_close.empty())
	{
		std::vector<context_ptr> tmp_storage;
		tmp_storage.reserve(to_close.size());
		while (!to_close.empty())
		{
			auto idx = to_close.top();
			to_close.pop();
			tmp_storage.emplace_back(std::exchange(m_contexts[idx], nullptr));
			m_contexts.erase(m_contexts.begin() + (int)idx);
		}
	}

	menu_bar();
}

void app::begin_menu_bar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
		ImGui::EndMenu();
	ImGui::EndMainMenuBar();
}

void app::menu_bar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit"))
			request_exit();
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Demo Window"))
			m_show_demo_window = !m_show_demo_window;
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Test"))
	{
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Trace"))
				LOG_TRACE("Trace");
			if (ImGui::MenuItem("Debug"))
				LOG_DEBUG("Debug");
			if (ImGui::MenuItem("Info"))
				LOG_INFO("Info");
			if (ImGui::MenuItem("Warn"))
				LOG_WARN("Warn");
			if (ImGui::MenuItem("Error"))
				LOG_ERROR("Error");
			if (ImGui::MenuItem("Critical"))
				LOG_CRITICAL("Critical");
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Assert"))
			ASSERT(false);

		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	if (m_show_demo_window)
		ImGui::ShowDemoWindow(&m_show_demo_window);
}