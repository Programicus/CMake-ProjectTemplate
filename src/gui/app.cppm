export module gui.app;

import core;
import gui.context;
import gui.vk.engine;

export namespace gui
{
	class app : public core::app
	{
	public:
		app(size_t argc, const char** argv, const char* appName = nullptr);
		app(const app&) = delete;
		app& operator=(const app&) = delete;
		virtual ~app(){}

		int run();
		void request_exit();

		template<typename T, typename... Args>
		T* emplace_context(Args&&... args)
			requires std::derived_from<T, context>
		{
			auto ret = new T(std::forward<Args>(args)...);
			m_contexts.emplace_back(ret);
			return ret;
		}

		virtual void welcome_menu() = 0;
		void focus(context* ctx);

	protected:
		void loop();
		virtual void update() = 0;
		void begin_menu_bar();
		virtual void menu_bar();

	private:
		VulkanEngine m_engine;
		std::vector<context_ptr> m_contexts;
		context* m_to_focus = nullptr;
		bool m_show_demo_window = false;
	};
}