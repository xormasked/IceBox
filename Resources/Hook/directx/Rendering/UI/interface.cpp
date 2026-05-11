#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../d3d11hook.hpp"

namespace Render {

auto user_interface() -> void
{
    if (!menu_open) return;

    ImGui::SetNextWindowSize(ImVec2(700, 520), ImGuiCond_FirstUseEver);
    ImGui::Begin("Ultraviolet.Paste", nullptr, ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Visuals")) {
            ImGui::Checkbox("Trace Lines", &visuals::Tracers);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.f);
            ImGui::Combo("##TracerPos", &visuals::TracerSelected, visuals::TracerPos, IM_ARRAYSIZE(visuals::TracerPos));
            ImGui::SameLine();
            ImGui::ColorEdit3("##TracerColor", &visuals::TracerColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputRGB);
            ImGui::Checkbox("Skeleton", &visuals::Skeleton);
            ImGui::SameLine();
            ImGui::ColorEdit3("##SkeletonColor", &visuals::SkeletonColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputRGB);
            ImGui::SliderFloat("Skeleton Thickness", &visuals::SkeletonThickness, 0.5f, 5.0f, "%.1f");
            ImGui::Checkbox("Team Check", &visuals::TeamCheck);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Misc")) {
            if (ImGui::Button("Uninject", ImVec2(150, 30))) {
                d3d11::should_uninject = true;
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

auto Background() -> void {}
auto AddMessage(const std::string&) -> void {}
auto Log() -> void {}
void RenderConsole() {}

} // namespace Render
