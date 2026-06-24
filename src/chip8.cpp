#include <stdint.h>
#include <SDL3/SDL.h>
#include "chip8.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <cstdlib>

#include <string.h>


Chip8::Chip8() : rd(), gen(rd()), distrib(0, 255) {
    std::memset(memory, 0, sizeof(memory));
    program_counter = 0x0200;
    stack_pointer = 0x00;
    std::memset(stack, 0, sizeof(stack));
    i_register = 0x0000;
    std::memset(vx, 0, sizeof(vx));
    delay_timer = 0;
    sound_timer = 0;
    chip8_pixels = renderer.getGrid();
    keys_state = renderer.getKeys();
}

Chip8::~Chip8(){
    if(chip8_pixels){
        chip8_pixels = nullptr;
    }
    if(keys_state){
        keys_state = nullptr;
    }
}

bool Chip8::chooseMethods(){
    std::cout << "8XY6 and 8XYE: Shift" << std::endl;
    std::cout << "0 for original COSMAC VIP, 1 for CHIP-48" << std::endl;
    std::cin >> shift_preference;
    std::cout << "BNNN: Jump with offset" << std::endl;
    std::cout << "0 for original COSMAC VIP(preferred), 1 for CHIP-48" << std::endl;
    std::cin >> jump_offset_preference;
    std::cout << "FX1E: Add to index" << std::endl;
    std::cout << "0 for original COSMAC VIP, 1 for CHIP-8 interpreter for Amiga (preferred)" << std::endl;
    std::cin >> add_to_index_preference;
    std::cout << "FX55 and FX65: Store and load memory" << std::endl;
    std::cout << "0 for original COSMAC VIP, 1 for CHIP-48 (preferred)" << std::endl;
    std::cin >> store_load_preference;
    return true;
}

bool Chip8::loadFont(){
    // 0
    memory[0x050] = 0xF0;
    memory[0x051] = 0x90;
    memory[0x052] = 0x90;
    memory[0x053] = 0x90;
    memory[0x054] = 0xF0;
    // 1
    memory[0x055] = 0x20;
    memory[0x056] = 0x60;
    memory[0x057] = 0x20;
    memory[0x058] = 0x20;
    memory[0x059] = 0x70;
    // 2
    memory[0x05A] = 0xF0;
    memory[0x05B] = 0x10;
    memory[0x05C] = 0xF0;
    memory[0x05D] = 0x80;
    memory[0x05E] = 0xF0;
    // 3
    memory[0x05F] = 0xF0;
    memory[0x060] = 0x10;
    memory[0x061] = 0xF0;
    memory[0x062] = 0x10;
    memory[0x063] = 0xF0;
    // 4
    memory[0x064] = 0x90;
    memory[0x065] = 0x90;
    memory[0x066] = 0xF0;
    memory[0x067] = 0x10;
    memory[0x068] = 0x10;
    // 5
    memory[0x069] = 0xF0;
    memory[0x06A] = 0x80;
    memory[0x06B] = 0xF0;
    memory[0x06C] = 0x10;
    memory[0x06D] = 0xF0;
    // 6
    memory[0x06E] = 0xF0;
    memory[0x06F] = 0x80;
    memory[0x070] = 0xF0;
    memory[0x071] = 0x90;
    memory[0x072] = 0xF0;
    // 7
    memory[0x073] = 0xF0;
    memory[0x074] = 0x10;
    memory[0x075] = 0x20;
    memory[0x076] = 0x40;
    memory[0x077] = 0x40;
    // 8
    memory[0x078] = 0xF0;
    memory[0x079] = 0x90;
    memory[0x07A] = 0xF0;
    memory[0x07B] = 0x90;
    memory[0x07C] = 0xF0;
    // 9
    memory[0x07D] = 0xF0;
    memory[0x07E] = 0x90;
    memory[0x07F] = 0xF0;
    memory[0x080] = 0x10;
    memory[0x081] = 0xF0;
    // A
    memory[0x082] = 0xF0;
    memory[0x083] = 0x90;
    memory[0x084] = 0xF0;
    memory[0x085] = 0x90;
    memory[0x086] = 0x90;
    // B
    memory[0x087] = 0xE0;
    memory[0x088] = 0x90;
    memory[0x089] = 0xE0;
    memory[0x08A] = 0x90;
    memory[0x08B] = 0xE0;
    // C
    memory[0x08C] = 0xF0;
    memory[0x08D] = 0x80;
    memory[0x08E] = 0x80;
    memory[0x08F] = 0x80;
    memory[0x090] = 0xF0;
    // D
    memory[0x091] = 0xE0;
    memory[0x092] = 0x90;
    memory[0x093] = 0x90;
    memory[0x094] = 0x90;
    memory[0x095] = 0xE0;
    // E
    memory[0x096] = 0xF0;
    memory[0x097] = 0x80;
    memory[0x098] = 0xF0;
    memory[0x099] = 0x80;
    memory[0x09A] = 0xF0;
    // F
    memory[0x09B] = 0xF0;
    memory[0x09C] = 0x80;
    memory[0x09D] = 0xF0;
    memory[0x09E] = 0x80;
    memory[0x09F] = 0x80;

    return true;
}

bool Chip8::loadBinary(std::string fileName){
    std::filesystem::path filePath(fileName);
    if(!std::filesystem::exists(filePath)){
        return false;
    }
    std::uintmax_t fileSize = std::filesystem::file_size(filePath);
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        return false;
    }
    file.read(reinterpret_cast<char*>(&memory[0x200]), fileSize);
    return true;
}

bool Chip8::loop(){
    if(!renderer.initializer()){
        return false;
    }
    //if(!loadBinary("./ROM/ibmlogo.ch8")){
    //if(!loadBinary("./ROM/test_opcode.ch8")){
    if(!loadBinary("./ROM/Tetris.ch8")){
    //if(!loadBinary("./ROM/Tictac.ch8")){
        return false;
    }
    bool is_running = true;
    Uint64 last_frame = SDL_GetTicksNS();
    Uint64 last_logic = SDL_GetTicksNS();
    const Uint64 FRAME_TARGET_NS = 1000000000ULL / 60;
    const Uint64 LOGIC_TARGET_NS = 1000000000ULL / 700;

    while(is_running){
        if(!renderer.eventHandler()){
            is_running = false;
            break;
        }
        Uint64 current_time = SDL_GetTicksNS();
        if(current_time - last_logic >= LOGIC_TARGET_NS){
            last_logic = current_time;
            instructionHandler();
        }
        if(current_time - last_frame >= FRAME_TARGET_NS){
            last_frame = current_time;
            renderer.iterator();
            if(delay_timer > 0) delay_timer--;

            if(sound_timer > 0){
                std::system("aplay sounds/beep.m4a &");
                sound_timer--;
            }
        }
        SDL_Delay(5);
    }
    return true;
}

void Chip8::instructionHandler(){
    uint16_t instruction = memory[program_counter] << 8;
    instruction = instruction | (memory[program_counter + 1]);
    uint16_t decoded_instruction = instruction & 0xF000;
    program_counter += 2;
    uint8_t x = (instruction & 0x0F00) >> 8;
    uint8_t y = (instruction & 0x00F0) >> 4;
    //std::cout << "pc: " << std::hex << program_counter << " instruction: " << std::hex << instruction << std::endl;
    //36 INSTRUCTIONS
    switch(decoded_instruction){
        case 0x0000: {
            if(instruction == 0x00E0){
                renderer.clearWindow();
            }else if(instruction == 0x00EE){
                program_counter = stack[stack_pointer];
                stack_pointer--;
            }
        } break;
        case 0x1000: {
            program_counter = instruction & 0x0FFF;
        } break;
        case 0x2000: {
            stack_pointer++;
            stack[stack_pointer] = program_counter;
            program_counter = instruction & 0x0FFF;
        } break;
        case 0x3000: {
            if(vx[x] == (instruction & 0x00FF)){
                program_counter += 2;
            }
        } break;
        case 0x4000: {
            if(vx[x] != (instruction & 0x00FF)){
                program_counter += 2;
            }
        } break;
        case 0x5000: {
            if(vx[x] == vx[y]){
                program_counter += 2;
            }
        } break;
        case 0x6000: {
            vx[x] = (instruction & 0x00FF);
        } break;
        case 0x7000: {
            vx[x] = vx[x] + (instruction & 0x00FF);
        } break;
        case 0x8000: {
            uint8_t last_nibble = instruction & 0x000F;
            switch(last_nibble){
                case 0x00:{
                    vx[x] = vx[y];
                } break;
                case 0x01:{
                    vx[x] = vx[x] | vx[y];
                } break;
                case 0x02:{
                    vx[x] = vx[x] & vx[y];
                } break;
                case 0x03:{
                    vx[x] = vx[x] ^ vx[y];
                } break;
                case 0x04:{
                    vx[0xF] = vx[x] + vx[y] > 255 ? 1 : 0;
                    vx[x] = vx[x] + vx[y];
                } break;
                case 0x05:{
                    vx[0xF] = vx[x] >= vx[y] ? 1 : 0;
                    vx[x] = vx[x] - vx[y];
                } break;
                case 0x06:{
                    vx[x] = vx[y]; // varies between programs
                    vx[0xF] = (vx[x] & 0x01);
                    vx[x] = vx[x] >> 1;
                } break;
                case 0x07:{
                    vx[0xF] = vx[y] >= vx[x] ? 1 : 0;
                    vx[x] = vx[y] - vx[x];
                } break;
                case 0x0E:{
                    vx[x] = vx[y]; // varies between programs
                    vx[0xF] = ((vx[x] & 0x80) >> 7);
                    vx[x] = vx[x] << 1;
                } break;
            }
        } break;
        case 0x9000: {
            if(vx[x] != vx[y]){
                program_counter += 2;
            }
        } break;
        case 0xA000: {
            i_register = (instruction & 0x0FFF);
        } break;
        case 0xB000: {
            program_counter = (instruction & 0x0FFF) + vx[0]; // varies between programs
        } break;
        case 0xC000: {
            uint8_t kk = (instruction & 0x00FF);
            vx[x] = kk & distrib(gen);
        } break;
        case 0xD000: {
            uint8_t x = vx[(instruction & 0x0F00) >> 8] % Constants::WINDOW_WIDTH;
            uint8_t y = vx[(instruction & 0x00F0) >> 4] % Constants::WINDOW_HEIGHT;
            uint8_t n = (instruction & 0x000F);
            vx[0xF] = 0;
            for(int i = 0; i < n; i++){
                for(int j = 0; j < 8; j++){
                    if(((memory[i_register + i] >> (7 - j)) & 0b00000001) == 1){
                        if(chip8_pixels[x + (y * Constants::WINDOW_WIDTH) + j + (i * Constants::WINDOW_WIDTH)] == 1){
                            chip8_pixels[x + (y * Constants::WINDOW_WIDTH) + j + (i * Constants::WINDOW_WIDTH)] = 0;
                            vx[0xF] = 1;
                        } else{
                            chip8_pixels[x + (y * Constants::WINDOW_WIDTH) + j + (i * Constants::WINDOW_WIDTH)] = 1;
                        }
                    }
                }
            }
        } break;
        case 0xE000: {
            uint8_t last_nibbles = instruction & 0x00FF;
            if(last_nibbles == 0x9E){
                if(keys_state[vx[x]]){
                    program_counter += 2;
                }
            }else if(last_nibbles == 0xA1){
                if(!keys_state[vx[x]]){
                    program_counter += 2;
                }
            }
        } break;
        case 0xF000: {
            uint8_t last_nibbles = instruction & 0x00FF;
            switch(last_nibbles){
                case 0x07: {
                    vx[x] = delay_timer;
                }break;
                case 0x0A: {
                    bool key_found = false;
                    for(int i = 0; i < 16; i++){
                        if(keys_state[i]){
                            vx[x] = i;
                            key_found = true;
                            break;
                        }
                    }
                    if(!key_found){
                        program_counter -= 2;
                    }
                }break;
                case 0x15: {
                    delay_timer = vx[x];
                }break;
                case 0x18: {
                    sound_timer = vx[x];
                }break;
                case 0x1E: {
                    i_register = i_register + vx[x];
                    vx[0xF] = i_register > 0x0FFF ? 1 : 0; // varies between programs
                }break;
                case 0x29: {
                    i_register = 0X050 + (vx[x] * 5);
                }break;
                case 0x33: {
                    memory[i_register] = vx[x] / 100;
                    memory[i_register + 1] = (vx[x] / 10) % 10;
                    memory[i_register + 2] = vx[x] % 10;
                }break;
                case 0x55: {
                    for(int i = 0; i <= x; i++){
                        memory[i_register + i] = vx[i]; // varies between programs
                    }
                }break;
                case 0x65: {
                    for(int i = 0; i <= x; i++){
                        vx[i] = memory[i_register + i]; // varies between programs
                    }
                }break;
            }
        } break;
    }
}