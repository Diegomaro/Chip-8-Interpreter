#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "renderer.hpp"

Renderer::Renderer(){
    window = nullptr;
    renderer = nullptr;
    texture = nullptr;
    std::memset(chip8_pixels, 0, sizeof(chip8_pixels));
    std::memset(screen_buffer, 0, sizeof(screen_buffer));
    std::memset(keys_state, 0, sizeof(keys_state));
}

Renderer::~Renderer(){
    if(texture){
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if(renderer){
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if(window){
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

bool Renderer::initializer(){
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }
    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator",
                                    Constants::WINDOW_WIDTH * Constants::SCALE,
                                    Constants::WINDOW_HEIGHT * Constants::SCALE,
                                    0,
                                    &window,
                                    &renderer)) {
        return false;
    }
    SDL_SetRenderLogicalPresentation(renderer, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    if (!texture) {
        return false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return true;
}

bool Renderer::clearWindow(){
    for(int i = 0; i < Constants::WINDOW_HEIGHT * Constants::WINDOW_WIDTH; i++){
        chip8_pixels[i] = 0;
    }
    return true;
}

uint8_t *Renderer::getGrid(){
    return chip8_pixels;
}

uint8_t *Renderer::getKeys(){
    return keys_state;
}

bool Renderer::iterator(){
    for(int i = 0; i < Constants::WINDOW_HEIGHT * Constants::WINDOW_WIDTH; i++){
        screen_buffer[i] = (chip8_pixels[i] == 1) ? 0xFFFFFFFF : 0x000000FF;
    }
    SDL_UpdateTexture(texture, nullptr, screen_buffer, Constants::WINDOW_WIDTH* sizeof(uint32_t));
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    return true;
}

bool Renderer::eventHandler(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if (event.type == SDL_EVENT_QUIT) return false;
        if(event.type == SDL_EVENT_KEY_DOWN || SDL_EVENT_KEY_UP){
            SDL_Scancode scancode = event.key.scancode;
            bool is_down = (event.type == SDL_EVENT_KEY_DOWN);
            switch(scancode){
                case SDL_SCANCODE_1:{
                    keys_state[1] = is_down;
                } break;
                case SDL_SCANCODE_2:{
                    keys_state[2] = is_down;
                } break;
                case SDL_SCANCODE_3:{
                    keys_state[3] = is_down;
                } break;
                case SDL_SCANCODE_4:{
                    keys_state[12] = is_down;
                } break;
                case SDL_SCANCODE_Q:{
                    keys_state[4] = is_down;
                } break;
                case SDL_SCANCODE_W:{
                    keys_state[5] = is_down;
                } break;
                case SDL_SCANCODE_E:{
                    keys_state[6] = is_down;
                } break;
                case SDL_SCANCODE_R:{
                    keys_state[13] = is_down;
                } break;
                case SDL_SCANCODE_A:{
                    keys_state[7] = is_down;
                } break;
                case SDL_SCANCODE_S:{
                    keys_state[8] = is_down;
                } break;
                case SDL_SCANCODE_D:{
                    keys_state[9] = is_down;
                } break;
                case SDL_SCANCODE_F:{
                    keys_state[14] = is_down;
                } break;
                case SDL_SCANCODE_Z:{
                    keys_state[10] = is_down;
                } break;
                case SDL_SCANCODE_X:{
                    keys_state[0] = is_down;
                } break;
                case SDL_SCANCODE_C:{
                    keys_state[11] = is_down;
                } break;
                case SDL_SCANCODE_V:{
                    keys_state[15] = is_down;
                } break;
            }
        }
    }
    return true;
}