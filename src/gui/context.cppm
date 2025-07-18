module;


#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <optional>
#include <string_view>
#include <string>
#include <unordered_map>

export module gui.context;

import core.app;
import gui.component;

export namespace gui
{
	export class context
	{
	public:
		context(std::string_view name)
		{
			set_name(name);
			focus_ctx();
		}
		context(const context&) = delete;
		context(context&&) = delete;
		context& operator=(const context&) = delete;
		context& operator=(context&&) = delete;
		virtual ~context() {}

		void loop(bool visible)
		{
			if (visible && !m_dockspace_id)
			{
				build_dock_space();
			}
			else if (!visible && m_dockspace_id)
			{
				ImGui::DockSpace(*m_dockspace_id, ImVec2(), ImGuiDockNodeFlags_KeepAliveOnly);
			}
			update(visible);
			for (auto& [_, component] : m_components)
			{
				component->loop();
			}
		}
		virtual void update(bool visible) = 0;
		void render()
		{
			ASSERT(m_dockspace_id, "Dockspace not built");

			ImGui::DockSpace(*m_dockspace_id);
			ImGui::SetNextWindowDockID(*m_dockspace_id);
			ImGui::Begin("central widget");
			render_gui();
			ImGui::End();

			for (auto& [_, component] : m_components)
			{
				component->render();
			}

			ImGui::BeginMainMenuBar();
			if (!m_components.empty() && ImGui::BeginMenu("Window"))
			{
				for (auto& [id, component] : m_components)
				{
					ImGui::Checkbox(id.c_str(), &component->m_open);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		virtual void render_gui() = 0;
		virtual void on_focus() {}
		virtual bool about_to_close() { return true; }

		virtual bool is_dirty() const { return false; }
		virtual bool can_close() const { return false; }

		const std::string_view name() const { return m_name; }

		void focus_ctx();

		template<typename T, typename... Args>
		T* add_component(component::id id, Args&&... args)
			requires std::derived_from<T, component>
		{
			T* ptr = nullptr;
			{
				auto comp = std::make_unique<T>(std::forward<Args>(args)...);
				ptr = comp.get();
				comp->m_owning_ctx = this;
				m_components[id] = std::move(comp);
			}
			return ptr;
		}

	protected:
		void set_name(std::string_view name) { m_name = name; }

	private:
		void build_dock_space()
		{
			ASSERT(!m_dockspace_id, "Dockspace already built");

			m_dockspace_id = ImGui::GetID("dock");
			ImGuiID dockspace_id = *m_dockspace_id;
			if (ImGui::DockBuilderGetNode(dockspace_id) != nullptr)
				return;
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_CentralNode);
		}

		std::string m_name;
		std::unordered_map<component::id, component_ptr> m_components;
		std::optional<ImGuiID> m_dockspace_id;
		bool m_visible = true;

		friend class app;
	};

	export using context_ptr = std::unique_ptr<context>;
}