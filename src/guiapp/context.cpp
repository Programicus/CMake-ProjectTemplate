#include <memory>

import core.app;
import gui.app;  // This import will work in the implementation file
import gui.context;

void gui::context::focus_ctx()
{
	using core_app = core::app;
	using gui_app = gui::app;
	core_app::GetApp<gui_app>().focus(this);
}