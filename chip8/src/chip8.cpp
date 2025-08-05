#include "chip8/chip8.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <type_traits>
#include <random>

Chip8::Chip8(const std::filesystem::path& rom_path) : 
    rom(rom_path, std::ios::in|std::ios::binary|std::ios::ate), PC(0x200), delay_timer(0), sound_timer(0), 
    display{}, RAM{}, keypad{},  waiting_for_key_release(false), stream(NULL) 
{
    background_color.r = 120;
    background_color.g = 140;
    background_color.b = 90;
    background_color.a = 100;

    draw_color.r = 255;
    draw_color.g = 255;
    draw_color.b = 255;
    draw_color.a = 100;

    key_bindings = {
        {SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2}, {SDL_SCANCODE_3, 0x3}, {SDL_SCANCODE_4, 0xC},
        {SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5}, {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xD},
        {SDL_SCANCODE_A, 0x7}, {SDL_SCANCODE_S, 0x8}, {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xE},
        {SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0x0}, {SDL_SCANCODE_C, 0xB}, {SDL_SCANCODE_V, 0xF}
    };
}

Chip8::~Chip8() {
    clean();
}

void Chip8::clearWindow() {
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);
}

void Chip8::init() {
    // TODO Handle return values
    SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO); 

    window = SDL_CreateWindow("Chip-8 Emulator", WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, NULL);
    clearWindow();

    const std::array<uint8_t, 80> font = {        
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(RAM.data(), font.data(), sizeof(font));

    if (rom.is_open()) {
        std::streampos size = rom.tellg();
        rom.seekg(0, std::ios::beg);
        rom.read(reinterpret_cast<char*>(RAM.data() + 0x200), size);
        rom.close();

        std::cout << "Read file into memory" << std::endl;
    } else {
        std::cout << "Unable to read in file" << std::endl;
    }

    configureSound();
}

void Chip8::clean() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    if (stream) SDL_DestroyAudioStream(stream);
    
    SDL_Quit();
}

std::string Chip8::get_memory_region_label(std::size_t address) const {
    if (address < 0x200) return "Reserved / Font";
    else if (address >= 0x200 && address < 0x600) return "ROM";
    else if (address >= 0x600 && address < 0xEA0) return "Free / Work RAM";
    else if (address >= 0xEA0 && address < 0xEFF) return "Reserved (impl.)";
    else if (address >= 0xF00 && address <= 0xFFF) return "Display Memory";
    return "";
}

void Chip8::showRamContent() const {
    for (size_t i = 0; i < RAM.size(); i += 16) {
        // Memory region label
        std::string region = get_memory_region_label(i);

        // Left address
        std::cout << "0x" << std::setw(3) << std::setfill('0') << std::hex << i << ": ";

        // Hex bytes
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < RAM.size()) {
                uint8_t byte = RAM[i + j];

                if (byte != 0) std::cout << "\033[33m"; // Yellow for non-zero
                else std::cout << "\033[90m";           // Dim gray for zero

                std::cout << std::setw(2) << static_cast<int>(byte) << " ";
            } else {
                std::cout << "   ";
            }
        }

        std::cout << "\033[0m"; // Reset color

        // ASCII view
        std::cout << " | ";
        for (size_t j = 0; j < 16 && i + j < RAM.size(); ++j) {
            char c = static_cast<char>(RAM[i + j]);
            std::cout << (std::isprint(c) ? c : '.');
        }

        // Region annotation
        std::cout << "  <-- " << region << '\n';
    }
}

void Chip8::renderDisplay() {
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (display[y][x]) {
                SDL_FRect pixel{
                    static_cast<float>(x) * SCALE,
                    static_cast<float>(y) * SCALE,
                    SCALE, SCALE
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void Chip8::handleInput(const SDL_Scancode& key, const Uint32& event_type) {
    if (event_type == SDL_EVENT_KEY_DOWN) {
        if (key == SDL_SCANCODE_ESCAPE)
            is_running = false;
        else if (key == SDL_SCANCODE_SPACE)
            is_paused ^= 1;
        else if (key_bindings.find(key) != key_bindings.end())
            keypad[key_bindings[key]] = 1;
    } else if (event_type == SDL_EVENT_KEY_UP) {
            if (key_bindings.find(key) != key_bindings.end())
                keypad[key_bindings[key]] = 0;
    }
}

int Chip8::current_sine_sample = 0;
void SDLCALL Chip8::FeedTheAudioStreamMore(void *userdata, SDL_AudioStream *astream, int additional_amount, int total_amount)
{
    additional_amount /= sizeof (float);
    while (additional_amount > 0) {
        float samples[128];
        const int total = SDL_min(additional_amount, SDL_arraysize(samples));
        int i;

        for (i = 0; i < total; i++) {
            const int freq = 440;
            const float phase = current_sine_sample * freq / 8000.0f;
            samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
            current_sine_sample++;
        }

        current_sine_sample %= 8000;

        SDL_PutAudioStreamData(astream, samples, total * sizeof (float));
        additional_amount -= total;
    }
}

void Chip8::configureSound() {
    SDL_AudioSpec spec;
    spec.channels = 1;
    spec.format = SDL_AUDIO_F32;
    spec.freq = 8000;
    
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, FeedTheAudioStreamMore, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    }
}

void Chip8::playSound() {
    SDL_ResumeAudioStreamDevice(stream);
}

void Chip8::stopSound() {
    SDL_PauseAudioStreamDevice(stream);
}

void Chip8::run() {
    init();

    is_running = true;
    is_paused = false;
    size_t instructions_per_frame = INSTRUCTION_PER_SECOND / FPS; 
    
    while (is_running) {
        
        SDL_Event event;        
        while (SDL_PollEvent(&event)) {  
            handleInput(event.key.scancode, event.type);
        }

        if (!is_paused) {
            for (size_t i = 0; i < instructions_per_frame; i++) {
                // Fetch and execute instructions
                uint16_t instruction = RAM[PC] << 8 | RAM[PC + 1];
                PC += 2;
                executeInstruction(instruction);
            }
            
            if (draw_to_screen)
                renderDisplay();        
        }

        if (delay_timer > 0)
            delay_timer--;

        if (sound_timer > 0) {
            sound_timer--;
            playSound();
        } else {
            stopSound();
        }
        
        fps_cap_timer.sleep();  
    }
}

void Chip8::executeInstruction(uint16_t instruction) {
    uint8_t first_nibble = instruction >> 12;

    uint16_t nnn = instruction & 0x0FFF;
    uint8_t n = instruction & 0x000F;
    uint8_t kk = instruction & 0x00FF;
    uint8_t x = (instruction & 0x0F00) >> 8;
    uint8_t y = (instruction & 0x00F0) >> 4;
    
    // std::cout << "Executing instruction: " << std::hex << instruction << std::endl;
    
    switch (first_nibble)
    {
    case 0x0:
        instr_set_0(instruction);
        break;
    case 0x1:
        instr_1nnn(nnn);
        break;
    case 0x2:
        instr_2nnn(nnn);
        break;
    case 0x3:
        instr_3xkk(x, kk);
        break;
    case 0x4:
        instr_4xkk(x, kk);
        break;
    case 0x5:
        instr_5xy0(x, y);
        break;
    case 0x6:
        instr_6xkk(x, kk);
        break;
    case 0x7:
        instr_7xkk(x, kk);
        break;
    case 0x8:
        instr_set_8(instruction, x, y);
        break;
    case 0x9:
        instr_9xy0(x, y);
        break;
    case 0xA:
        instr_Annn(nnn);
        break;
    case 0xB:
        instr_Bnnn(nnn);
        break;
    case 0xC:
        instr_Cxkk(x, kk);
        break;
    case 0xD:
        instr_Dxyn(x, y, n);
        break;
    case 0xE:
        instr_set_E(instruction, x);
        break;
    case 0xF:
        instr_set_F(instruction, x);
        break;
    default:
        std::cout << "Invalid instruction" << std::endl;
        break;
    }
}

void Chip8::instr_set_0(uint16_t instruction) {
    switch (instruction)
    {
    case 0x00E0:
        instr_00E0();
        break;
    case 0x00EE:
        instr_00EE();
        break;
    default:
        break;
    }
}

void Chip8::instr_set_8(uint16_t instruction, uint8_t x, uint8_t y) {
    uint8_t last_nibble = instruction & 0x000F;
    switch (last_nibble)
    {
    case 0x0:
        instr_8xy0(x, y);
        break;
    case 0x1:
        instr_8xy1(x, y);
        break;
    case 0x2:
        instr_8xy2(x, y);
        break;
    case 0x3:
        instr_8xy3(x, y);
        break;
    case 0x4:
        instr_8xy4(x, y);
        break;
    case 0x5:
        instr_8xy5(x, y);
        break;
    case 0x6:
        instr_8xy6(x, y);
        break;
    case 0x7:
        instr_8xy7(x, y);
        break;
    case 0xE:
        instr_8xyE(x, y);
        break;
    default:
        break;
    } 
}

void Chip8::instr_set_E(uint16_t instruction, uint8_t x) {
    uint8_t last_two_nibbles = instruction & 0xFF;
    switch (last_two_nibbles)
    {
    case 0x9E:
        instr_Ex9E(x);
        break;
    case 0xA1:
        instr_ExA1(x);
        break;
    default:
        break;
    }
}

void Chip8::instr_set_F(uint16_t instruction, uint8_t x) {
    uint8_t last_two_nibbles = instruction & 0xFF;
    switch (last_two_nibbles)
    {
    case 0x07:
        instr_Fx07(x);
        break;
    case 0x0A:
        instr_Fx0A(x);
        break;
    case 0x15:
        instr_Fx15(x);
        break;
    case 0x18:
        instr_Fx18(x);
        break;
    case 0x1E:
        instr_Fx1E(x);
        break;
    case 0x29:
        instr_Fx29(x);
        break;
    case 0x33:
        instr_Fx33(x);
        break;
    case 0x55:
        instr_Fx55(x);
        break;
    case 0x65:
        instr_Fx65(x);
        break;
    default:
        break;
    }
}


void Chip8::instr_00E0() {
    std::fill(display.begin(), display.end(), std::array<bool, WINDOW_WIDTH>{});
    clearWindow();
}

void Chip8::instr_00EE() {
    PC = stack.top();
    stack.pop();
} 

void Chip8::instr_0nnn(uint16_t nnn) {
    // Do not implement
}

void Chip8::instr_1nnn(uint16_t nnn) {
    PC = nnn;
}

void Chip8::instr_2nnn(uint16_t nnn) {
    stack.push(PC);
    PC = nnn;
}

void Chip8::instr_3xkk(uint8_t x, uint8_t kk) {
    if (V[x] == kk) 
        PC += 2;
}

void Chip8::instr_4xkk(uint8_t x, uint8_t kk) {
    if (V[x] != kk) 
        PC += 2;
}

void Chip8::instr_5xy0(uint8_t x, uint8_t y) {
    if (V[x] == V[y]) 
        PC += 2;
}

void Chip8::instr_6xkk(uint8_t x, uint8_t kk) {
    V[x] = kk;
}

void Chip8::instr_7xkk(uint8_t x, uint8_t kk) {
    V[x] += kk;
}

void Chip8::instr_8xy0(uint8_t x, uint8_t y) {
    V[x] = V[y];
}

void Chip8::instr_8xy1(uint8_t x, uint8_t y) {
    V[x] |= V[y];
}

void Chip8::instr_8xy2(uint8_t x, uint8_t y) {
    V[x] &= V[y];
}

void Chip8::instr_8xy3(uint8_t x, uint8_t y) {
    V[x] ^= V[y];
}

void Chip8::instr_8xy4(uint8_t x, uint8_t y) {
    uint16_t result = V[x] + V[y];
    V[x] = result & 0x00FF;
    V[0xF] = result > 0xFF;
}

void Chip8::instr_8xy5(uint8_t x, uint8_t y) {
    uint8_t bit = V[x] >= V[y]; 
    V[x] -= V[y];
    V[0xF] = bit;
}

void Chip8::instr_8xy6(uint8_t x, uint8_t y) {
    uint8_t bit = V[x] & 0x1;
    V[x] >>= 1;
    V[0xF] = bit;
}

void Chip8::instr_8xy7(uint8_t x, uint8_t y) {
    uint8_t bit = V[y] >= V[x];
    V[x] = V[y] - V[x];
    V[0xF] = bit;
}

void Chip8::instr_8xyE(uint8_t x, uint8_t y) {
    uint8_t bit = (V[x] & 0x80) >> 7;
    V[x] <<= 1;
    V[0xF] = bit;
}

void Chip8::instr_9xy0(uint8_t x, uint8_t y) {
    if (V[x] != V[y])
        PC += 2;
}

void Chip8::instr_Annn(uint16_t nnn) {
    I = nnn;
}

void Chip8::instr_Bnnn(uint16_t nnn) {
    PC = nnn + V[0];
}

void Chip8::instr_Cxkk(uint8_t x, uint8_t kk) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 255); // define the range

    V[x] = distr(gen) & kk;
} 

void Chip8::instr_Dxyn(uint8_t x, uint8_t y, uint8_t n) {
    uint8_t start_x = V[x] % 64;
    uint8_t start_y = V[y] % 32;
    
    V[0xF] = 0;

    for (uint8_t i = 0; i < n; i++)
    {   
        if (start_y + i >= 32) break;

        if (I + i >= RAM.size()) {
            SDL_Log("Dxyn: RAM read out of bounds at I + %u (0x%03X)", i, I + i);
            break;
        }

        uint8_t sprite_byte = RAM[I + i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (start_x + j >= 64) break;

            uint8_t bit = (sprite_byte >> (7 - j)) & 1;

            if (bit) {
                if (display[start_y + i][start_x + j]) {
                    V[0xF] = 1;
                    display[start_y + i][start_x + j] = 0;
                } else {
                    display[start_y + i][start_x + j] = 1;
                }
            }
        }      
    }

    draw_to_screen = true;
}    

void Chip8::instr_Ex9E(uint8_t x) {
    if (keypad[V[x] & 0xF]) {
        PC += 2;
    }
}

void Chip8::instr_ExA1(uint8_t x) {
    if (!keypad[V[x] & 0xF]) {
        PC += 2;
    }
}

void Chip8::instr_Fx07(uint8_t x) {
    V[x] = delay_timer;
}

void Chip8::instr_Fx0A(uint8_t x) {
    if (!waiting_for_key_release) {
        for (uint8_t i = 0; i < 16; i++) {
            if (keypad[i & 0xF]) {
                V[x] = i;
                waiting_for_key_release = true;
                PC -= 2;
                return;
            }
        }
        PC -= 2;
    } else {
        bool all_released = true;
        for (uint8_t i = 0; i < 16; i++) {
            if (keypad[i & 0xF]) {
                all_released = false;
                break;
            }
        }

        if (all_released) {
            waiting_for_key_release = false;
        } else {
            PC -= 2;
        }
    }
}

void Chip8::instr_Fx15(uint8_t x) {
    delay_timer = V[x];
}

void Chip8::instr_Fx18(uint8_t x) {
    sound_timer = V[x];
}

void Chip8::instr_Fx1E(uint8_t x) {
    I += V[x];
}

void Chip8::instr_Fx29(uint8_t x) {
    I = V[x] * 5;
}

void Chip8::instr_Fx33(uint8_t x) {
    if (I + 2 >= RAM.size()) {
        SDL_Log("Fx33: Memory write would exceed RAM bounds! (I = 0x%03X)", I);
        return;
    }

    RAM[I] = V[x] / 100;
    RAM[I + 1] = (V[x] / 10) % 10;
    RAM[I + 2] = V[x] % 10;
}

void Chip8::instr_Fx55(uint8_t x) {
    for (uint8_t i = 0; i <= x; i++) {
        if (I + i >= RAM.size()) {
            return;
        }
        RAM[I + i] = V[i];
    }
}

void Chip8::instr_Fx65(uint8_t x) {
    for (uint8_t i = 0; i <= x; i++) {
        if (I + i >= RAM.size()) {
            return;
        }

        V[i] = RAM[I + i];
    }
}
