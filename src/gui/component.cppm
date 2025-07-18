module;

#include <memory>
#include <string>

#include <imgui.h>

export module gui.component;

export namespace gui
{
	class context;

	class component
	{
	public:
		using id = std::string;

		component() {}
		component(const component&) = delete;
		component(component&&) = delete;
		component& operator=(const component&) = delete;
		component& operator=(component&&) = delete;
		virtual ~component() {}

		void loop()
		{
			update();
		}

		virtual void update() = 0;
		void render()
		{
			if (!m_open)
				return;
			ImGui::Begin(m_name.data(), &m_open);
			render_gui();
			ImGui::End();
		}
		virtual void render_gui() = 0;

		const std::string_view name() const { return m_name; }
		bool is_open() const { return m_open; }

	protected:
		void set_name(std::string_view name) { m_name = name; }
		template<typename T = context>
		T* get_ctx() const { ASSERT(m_owning_ctx);  return static_cast<T*>(m_owning_ctx); }

	private:

		std::string m_name;
		context* m_owning_ctx = nullptr;
		bool m_open = true;

		friend class GuiApp;
		friend class context;
	};

	using component_ptr = std::unique_ptr<component>;
}