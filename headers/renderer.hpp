#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "constants.hpp"

class Renderer{
    public:
        Renderer();
        ~Renderer();
        bool initializer();
        bool iterator();
        bool eventHandler();
        bool clearWindow();
        uint8_t *getGrid();
        uint8_t *getKeys();
    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
        uint8_t chip8_pixels[Constants::WINDOW_WIDTH * Constants::WINDOW_HEIGHT];
        uint32_t screen_buffer[Constants::WINDOW_WIDTH * Constants::WINDOW_HEIGHT];
        uint8_t keys_state[16];
};