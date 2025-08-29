#include "gui/debugger.hpp"
#include "imgui.h"

void Debugger::showRegisters(std::array<uint8_t, 16>& registers) {
    if (!ImGui::Begin("Registers")) {
        ImGui::End();
        return;
    }
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
    if (ImGui::BeginTable("Registers", 2, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("V[x]", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        // Demonstrate using clipper for large vertical lists
        ImGuiListClipper clipper;
        clipper.Begin(registers.size());
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("V[%d]", row);
            
                // ImGui::Text("Dec: %d|Hex: %X", registers.at(row), registers.at(row));
                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(row);
                
                int value = registers.at(row); // promote to int so InputInt works
                if (ImGui::InputScalar("##reg", ImGuiDataType_U8, &value,
                       nullptr, nullptr, "%02X",
                       ImGuiInputTextFlags_CharsHexadecimal)) {
                    registers.at(row) = static_cast<uint8_t>(value);
                }

                ImGui::PopID();
            }
        }
        ImGui::EndTable();

    }
        
    ImGui::End();
}

void showTimers() {

}

void showProgrammCounter() {

}
