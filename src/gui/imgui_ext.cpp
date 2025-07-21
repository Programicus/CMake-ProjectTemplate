#include "imgui_ext.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace ImGuiExt
{
    void ForceNewViewport()
    {
        // Force creation of a new viewport using ImGuiWindowClass with NoAutoMerge flag
        // This is the same mechanism that GetWindowAlwaysWantOwnViewport() checks for
        static ImGuiWindowClass window_class;
        window_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
        
        ImGui::SetNextWindowClass(&window_class);
    }
    
    void ForceNewViewportAlwaysOnTop()
    {
        // Force creation of a new always-on-top viewport
        // Uses both NoAutoMerge (to force new viewport) and TopMost (for always-on-top)
        static ImGuiWindowClass window_class;
        window_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge | ImGuiViewportFlags_TopMost;
        
        ImGui::SetNextWindowClass(&window_class);
    }
}