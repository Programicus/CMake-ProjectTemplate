module gui.context.welcome;

import core;
import gui.app;

void gui::welcome_ctx::render_gui()
{
	core::app::GetApp<gui::app>().welcome_menu();
}