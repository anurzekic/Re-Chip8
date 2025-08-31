#include "gui/debugger.hpp"

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

void Debugger::showTimers(uint16_t& PC, uint16_t& I, uint8_t& delay_timer, uint8_t& sound_timer) {
    if (!ImGui::Begin("Special Registers")) {
        ImGui::End();
        return;
    }
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 4);
    if (ImGui::BeginTable("Registers", 2, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Register", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Sound Timer");
        insertEditableValue(sound_timer, 0);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Delay Timer");
        insertEditableValue(delay_timer, 1);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("I");
        insertEditableValue(I, 2);

        ImGui::EndTable();
    }        

    ImGui::End();
}

void Debugger::showProgramCounter(uint16_t& PC) {
    // TODO Add instruction reversal (the timers and stuff)
    if (!ImGui::Begin("Program Counter")) {
        ImGui::End();
        return;
    }

    int programCounter = PC;

    // -2 button
    if (ImGui::Button("-2")) {
        PC -= 4;
    }
    ImGui::SameLine();

    // -1 button
    if (ImGui::Button("-1")) {
        PC -= 2;
    }
    ImGui::SameLine();

    // Editable input
    ImGui::PushItemWidth(100); // make it narrower
    ImGui::InputScalar("##reg", ImGuiDataType_U8, &programCounter,
                       nullptr, nullptr, "%04X",
                       ImGuiInputTextFlags_CharsHexadecimal);    
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // +1 button
    if (ImGui::Button("+1")) {
        PC += 2;
    }
    ImGui::SameLine();

    // +2 button
    if (ImGui::Button("+2")) {
        PC += 4;
    }

    ImGui::End();
}

void Debugger::showStack(std::vector<uint16_t>& stack) {
    if (!ImGui::Begin("Stack")) {
        ImGui::End();
        return;
    }

    createTable("Stack", "Stack", "Value", stack);

    ImGui::End();
}

void Debugger::showKeypad(std::array<bool, 16>& keypad) {
    if (!ImGui::Begin("Keypad")) {
        ImGui::End();
        return;
    }

    createTable("Keypad", "Key", "Value", keypad);

    ImGui::End();
}

