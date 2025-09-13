#pragma once

#include "imgui.h"

#include <array>
#include <cstdint>
#include <vector>
#include <string>

class Debugger {
public:
    Debugger() = default;
    ~Debugger() = default;
    Debugger(const Debugger&) = default;

    void showRegisters(std::array<uint8_t, 16>& registers);
    void showTimers(uint16_t& PC, uint16_t& I, uint8_t& delay_timer, uint8_t& sound_timer);
    void showProgramCounter(uint16_t& PC, std::array<uint8_t, 4096>& RAM);
    void showStack(std::vector<uint16_t>& stack);
    void showKeypad(std::array<bool, 16>& keypad);
    std::string disassemble(uint16_t opcode);

private:
    template<typename InputT>
    void insertEditableValue(InputT& value, int id) {
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(id);
        
        int st = value; // promote to int so InputInt works
        if (ImGui::InputScalar("##reg", ImGuiDataType_U8, &st,
                nullptr, nullptr, "%02X",
                ImGuiInputTextFlags_CharsHexadecimal)) {
            value = static_cast<InputT>(st);
        }

        ImGui::PopID();
    } 
    
    template<typename InputT>
    void createTable(const char* table_name, const char* first_column_name, const char* second_column_name, InputT& data) {
        
        static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
        ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
        if (ImGui::BeginTable(table_name, 2, flags, outer_size))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn(first_column_name, ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn(second_column_name, ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;
            clipper.Begin(data.size());
            while (clipper.Step())
            {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s[%d]", first_column_name, row);
                
                    // ImGui::Text("Dec: %d|Hex: %X", registers.at(row), registers.at(row));
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushID(row);
                    
                    int value = data.at(row); // promote to int so InputInt works
                    if (ImGui::InputScalar("##reg", ImGuiDataType_U16, &value,
                        nullptr, nullptr, "%04X",
                        ImGuiInputTextFlags_CharsHexadecimal)) {
                        data.at(row) = static_cast<decltype(value)>(value);
                    }

                    ImGui::PopID();
                }
            }
            ImGui::EndTable();

        }
    }
};
