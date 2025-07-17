module;

#include <utility>

export module core.scope_guard;

export namespace core
{
	template<typename T>
	class scope_guard
	{
	public:
		scope_guard(T&& func) : m_func(std::move(func)) {}
		scope_guard(const T& func) : m_func(func) {}
		scope_guard(scope_guard&&) = default;

		scope_guard() = delete;
		scope_guard(const scope_guard&) = delete;
		scope_guard& operator=(const scope_guard&) = delete;
		scope_guard& operator=(scope_guard&&) = delete;

		~scope_guard() { m_func(); }
	private:
		T m_func;
	};

	template<typename T>
	scope_guard(T&&) -> scope_guard<T>;

	template<typename T>
	scope_guard<T> make_scope_guard(T&& func)
	{
		return scope_guard(std::forward<T>(func));
	}
}