#pragma once
#include <stdint.h>
#include "renderer.hpp"
#include <random>

class Chip8{
    public:
        Chip8();
        ~Chip8();
        bool chooseMethods();
        bool loadFont();
        bool loadBinary(std::string fileName);
        bool loop();
        void instructionHandler();
    private:
        Renderer renderer;
        uint8_t memory[4096];
        uint16_t program_counter;
        uint8_t stack_pointer;
        uint16_t stack [16];
        uint16_t i_register;
        uint8_t vx [16];
        uint8_t delay_timer;
        uint8_t sound_timer;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_int_distribution<int> distrib;

        uint8_t *chip8_pixels;
        uint8_t *keys_state;

        bool shift_preference;
        bool jump_offset_preference;
        bool add_to_index_preference;
        bool store_load_preference;
};