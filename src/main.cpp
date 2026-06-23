#include <SDL3/SDL.h>
#include "chip8.hpp"
#include "renderer.hpp"

int main(){
    Chip8 emulator;
    return emulator.loop();
}