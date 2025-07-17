module;

#include <spdlog/spdlog.h>

#include <span>
#include <vector>

export module core.app;

export namespace core
{

	export class app
	{
	public:
		app(size_t argc, const char** argv);
		app(const app&) = delete;
		app& operator=(const app&) = delete;
		~app();

		int run(); //Intentionally left unimplemented, that is for subclasses to implement

		static app& GetApp();

		template<typename T>
		static T& GetApp()
			requires std::derived_from<T, app>
		{
			return static_cast<T&>(GetApp());
		}

		template<typename T, typename... Args>
		spdlog::sink_ptr AddSink(const char* name, Args&&... args)
		{
			auto logger = std::make_shared<T>(name, std::forward<Args>(args)...);
			spdlog::sink_ptr ret(logger, logger.get());
			this->m_sinks.push_back(ret);
			spdlog::default_logger()->sinks().push_back(ret);
			return ret;
		}

	protected:
		std::span<const char*> m_args;

	private:
		static app* s_instance;
		std::vector<spdlog::sink_ptr> m_sinks;
	};
}