export module gui.context.welcome;

import gui.context;

export namespace gui
{
	class welcome_ctx : public context
	{
	public:
		welcome_ctx() : context("welcome") {}
		welcome_ctx(const welcome_ctx&) = delete;
		welcome_ctx(welcome_ctx&&) = delete;
		welcome_ctx& operator=(const welcome_ctx&) = delete;
		welcome_ctx& operator=(welcome_ctx&&) = delete;
		virtual ~welcome_ctx() {}
		virtual void update(bool /*visible*/) override {}
		virtual void render_gui() override;
	};
}