#pragma once

#include <spdlog/fmt/fmt.h>

#include <imgui.h>

#include <string_view>

namespace ImGuiExt
{
	class IdPusher
	{
	public:
		IdPusher(const char* id)
		{
			ImGui::PushID(id);
		}
		IdPusher(const void* id)
		{
			ImGui::PushID(id);
		}
		~IdPusher()
		{
			ImGui::PopID();
		}
	};

	inline void Text(std::string_view str)
	{
		ImGui::TextUnformatted(str.data(), str.data() + str.size());
	}

	inline void Text(const char* str)
	{
		ImGui::TextUnformatted(str);
	}

	template<typename... Args>
	void Text(fmt::format_string<Args...> fmt, Args&&... args)
	{
		auto text = fmt::format(fmt, std::forward<Args>(args)...);
		ImGui::Text("%s", text.c_str());
	}

	// Forces the next ImGui window to be created in a new viewport (separate GLFW window)
	void ForceNewViewport();
}