import core;
import gui;

void gui::welcome_ctx::render_gui()
{
	core::app::GetApp<gui::app>().welcome_menu();
}