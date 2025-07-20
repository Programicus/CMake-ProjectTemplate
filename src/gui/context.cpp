module gui.context;

import core.app;
import gui.app;

void gui::context::focus_ctx()
{
	using core_app = core::app;
	using gui_app = gui::app;
	core_app::GetApp<gui_app>().focus(this);
}