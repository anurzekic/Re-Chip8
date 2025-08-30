#pragma once

#include "imgui.h"

#include <array>
#include <cstdint>

class Debugger {
public:
    Debugger() = default;
    ~Debugger() = default;
    Debugger(const Debugger&) = default;

    void showRegisters(std::array<uint8_t, 16>& registers);
    void showTimers(uint16_t& PC, uint16_t& I, uint8_t& delay_timer, uint8_t& sound_timer);
    void showProgrammCounter();

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

};
