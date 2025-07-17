module;

#include "imgui_ext.h"

#include <imgui.h>
#include <glfw/glfw3.h>

#include <memory>
#include <filesystem>
#include <stack>
#include <ranges>
#include <vector>

export module gui.app;

import core;
import gui.context;
import gui.context.welcome;
import gui.vk.engine;

namespace fs = std::filesystem;

export namespace gui
{
	class app : public core::app
	{
	public:
		app(size_t argc, const char** argv, const char* appName = nullptr)
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
		app(const app&) = delete;
		app& operator=(const app&) = delete;
		virtual ~app(){}

		int run()
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

		void request_exit()
		{
			glfwSetWindowShouldClose(m_engine.window, GLFW_TRUE);
		}

		template<typename T, typename... Args>
		T* emplace_context(Args&&... args)
			requires std::derived_from<T, context>
		{
			auto ret = new T(std::forward<Args>(args)...);
			m_contexts.emplace_back(ret);
			return ret;
		}

		virtual void welcome_menu() = 0;

		void focus(context* ctx)
		{
			m_to_focus = ctx;
		}

	protected:
		void loop()
		{
			begin_menu_bar();
			ImGui::BeginTabBar("contexts");
			bool first = true;
			std::stack<size_t> to_close;
			for (auto [idx, ctx] : m_contexts | std::ranges::views::enumerate)
			{
				ImGuiExt::IdPusher id(ctx.get());

				auto name = ctx->name();

				bool* closeable = first ? nullptr : &(ctx->m_visible);
				ImGuiTabBarFlags flags = 0;
				if (first)
					flags |= ImGuiTabItemFlags_Leading;

				if (ctx.get() == m_to_focus)
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
			m_to_focus = nullptr;

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

		virtual void update() = 0;
		void begin_menu_bar()
		{
			ImGui::BeginMainMenuBar();
			if (ImGui::BeginMenu("File"))
				ImGui::EndMenu();
			ImGui::EndMainMenuBar();
		}

		virtual void menu_bar()
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

	private:
		VulkanEngine m_engine;
		std::vector<context_ptr> m_contexts;
		context* m_to_focus = nullptr;
		bool m_show_demo_window = false;
	};
}