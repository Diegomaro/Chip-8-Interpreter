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

    /*
    chip8_pixels[0] = 1;
    chip8_pixels[(16 * Constants::WINDOW_WIDTH) + 32] = 1;
    chip8_pixels[(Constants::WINDOW_HEIGHT - 1) * Constants::WINDOW_WIDTH] = 1;
    */
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
        if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN)
        return false;
    }
    return true;
}