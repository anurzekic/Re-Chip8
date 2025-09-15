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

std::string Debugger::disassemble(uint16_t opcode) {
    uint16_t nnn = opcode & 0x0FFF;      // address
    uint8_t  nn  = opcode & 0x00FF;      // 8-bit constant
    uint8_t  n   = opcode & 0x000F;      // 4-bit constant
    uint8_t  x   = (opcode >> 8) & 0x0F; // register X
    uint8_t  y   = (opcode >> 4) & 0x0F; // register Y

    char buf[64];

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: return "CLS";
                case 0x00EE: return "RET";
                default: snprintf(buf, sizeof(buf), "SYS %03X", nnn); return buf;
            }
        case 0x1000: snprintf(buf, sizeof(buf), "JP %03X", nnn); return buf;
        case 0x2000: snprintf(buf, sizeof(buf), "CALL %03X", nnn); return buf;
        case 0x3000: snprintf(buf, sizeof(buf), "SE V%X, %02X", x, nn); return buf;
        case 0x4000: snprintf(buf, sizeof(buf), "SNE V%X, %02X", x, nn); return buf;
        case 0x5000: snprintf(buf, sizeof(buf), "SE V%X, V%X", x, y); return buf;
        case 0x6000: snprintf(buf, sizeof(buf), "LD V%X, %02X", x, nn); return buf;
        case 0x7000: snprintf(buf, sizeof(buf), "ADD V%X, %02X", x, nn); return buf;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0: snprintf(buf, sizeof(buf), "LD V%X, V%X", x, y); return buf;
                case 0x1: snprintf(buf, sizeof(buf), "OR V%X, V%X", x, y); return buf;
                case 0x2: snprintf(buf, sizeof(buf), "AND V%X, V%X", x, y); return buf;
                case 0x3: snprintf(buf, sizeof(buf), "XOR V%X, V%X", x, y); return buf;
                case 0x4: snprintf(buf, sizeof(buf), "ADD V%X, V%X", x, y); return buf;
                case 0x5: snprintf(buf, sizeof(buf), "SUB V%X, V%X", x, y); return buf;
                case 0x6: snprintf(buf, sizeof(buf), "SHR V%X", x); return buf;
                case 0x7: snprintf(buf, sizeof(buf), "SUBN V%X, V%X", x, y); return buf;
                case 0xE: snprintf(buf, sizeof(buf), "SHL V%X", x); return buf;
                default: return "UNKNOWN";
            }
        case 0x9000: snprintf(buf, sizeof(buf), "SNE V%X, V%X", x, y); return buf;
        case 0xA000: snprintf(buf, sizeof(buf), "LD I, %03X", nnn); return buf;
        case 0xB000: snprintf(buf, sizeof(buf), "JP V0, %03X", nnn); return buf;
        case 0xC000: snprintf(buf, sizeof(buf), "RND V%X, %02X", x, nn); return buf;
        case 0xD000: snprintf(buf, sizeof(buf), "DRW V%X, V%X, %X", x, y, n); return buf;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E: snprintf(buf, sizeof(buf), "SKP V%X", x); return buf;
                case 0xA1: snprintf(buf, sizeof(buf), "SKNP V%X", x); return buf;
                default: return "UNKNOWN";
            }
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07: snprintf(buf, sizeof(buf), "LD V%X, DT", x); return buf;
                case 0x0A: snprintf(buf, sizeof(buf), "LD V%X, K", x); return buf;
                case 0x15: snprintf(buf, sizeof(buf), "LD DT, V%X", x); return buf;
                case 0x18: snprintf(buf, sizeof(buf), "LD ST, V%X", x); return buf;
                case 0x1E: snprintf(buf, sizeof(buf), "ADD I, V%X", x); return buf;
                case 0x29: snprintf(buf, sizeof(buf), "LD F, V%X", x); return buf;
                case 0x33: snprintf(buf, sizeof(buf), "LD B, V%X", x); return buf;
                case 0x55: snprintf(buf, sizeof(buf), "LD [I], V0-V%X", x); return buf;
                case 0x65: snprintf(buf, sizeof(buf), "LD V0-V%X, [I]", x); return buf;
                default: return "UNKNOWN";
            }
        default:
            return "UNKNOWN";
    }
}

void Debugger::showProgramCounter(uint16_t& PC, const std::array<uint8_t, 4096>& RAM, Chip8& chip8) {
    if (!ImGui::Begin("Program Counter")) {
        ImGui::End();
        return;
    }

    float row_height = 25.0f;
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    if (ImGui::BeginTable("KeypadTable", 3, flags)) {
        ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
        
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("Run")) {
            chip8.is_paused = false; // Toggle pause state
        }

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Pause")) {
            chip8.is_paused = true; // Toggle pause state
        } 

        ImGui::TableSetColumnIndex(2);
        if (ImGui::Button("Step")) {
            chip8.step();
        } 

        ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);

        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("-2")) PC = (PC >= 4) ? PC - 4 : 0x200;
        ImGui::SameLine();
        if (ImGui::Button("-1")) PC = (PC >= 2) ? PC - 2 : 0x200;

        ImGui::TableSetColumnIndex(1);
        uint16_t opcode = (RAM[PC] << 8) | RAM[PC + 1];
        ImGui::Text("PC: %03X", PC);

        ImGui::TableSetColumnIndex(2);
        if (ImGui::Button("+1")) PC += 2;
        ImGui::SameLine();
        if (ImGui::Button("+2")) PC += 4;


        ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);

        if (PC + 1 < RAM.size()) {
            uint16_t opcode = (RAM[PC] << 8) | RAM[PC + 1];
            
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Instruction:");

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%04X", opcode);
            
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", disassemble(opcode).c_str());
        } else {
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Instruction:");

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("<out of range>");
        }

        ImGui::EndTable();
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

    static const int layout[16] = {
        0x1, 0x2, 0x3, 0xC,
        0x4, 0x5, 0x6, 0xD,
        0x7, 0x8, 0x9, 0xE,
        0xA, 0x0, 0xB, 0xF
    };

    if (ImGui::BeginTable("KeypadTable", 4, ImGuiTableFlags_Borders)) {
        for (int i = 0; i < 16; i++) {
            if (i % 4 == 0)
                ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(i % 4);

            int key = layout[i];
            ImGui::PushID(key);

            bool pressed = keypad[key];
            char label[3];
            snprintf(label, sizeof(label), "%X", key); // show as hex (0–F)

            if (ImGui::Selectable(label, pressed, ImGuiSelectableFlags_None, ImVec2(40, 40))) {
                keypad[key] = !pressed;
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void Debugger::showDisassembly(const uint16_t& PC, const std::array<uint8_t, 4096>& RAM, long rom_size) {
    if (!ImGui::Begin("Disassembly")) {
        ImGui::End();
        return;
    }
    
    static bool follow_pc = true;
    ImGui::BeginChild("DisassemblyToolbar", ImVec2(0, 20), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Checkbox("Follow PC", &follow_pc);
    ImGui::EndChild();

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    ImGui::BeginChild("DisassemblyContent", ImVec2(0, 0), true);
    if (ImGui::BeginTable("DissasemblyTable", 3, flags)) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Hex", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Instruction", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        for (int row = 0; row < rom_size / 2; row++) {
            int index = 0x200 + row * 2;
            if (index + 1 >= RAM.size())
                break;

            uint16_t opcode = (RAM[index] << 8) | RAM[index + 1];

            ImGui::TableNextRow();
            if (index == PC) {
                ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, color); 
                
                if (follow_pc)
                    ImGui::SetScrollHereY(0.5f);
            }

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%03X", index);
        
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%04X", opcode);
            
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", disassemble(opcode).c_str());
        }

        ImGui::EndTable();
    }
    ImGui::EndChild();

    ImGui::End();
}
