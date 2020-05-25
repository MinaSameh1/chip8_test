#ifndef PLATFORM_H
#define PLATFORM_H

#include "chip8.h"
#include<SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

void Chip8_start(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

void Exit();

void Update(void const* buffer, int pitch);

int ProcessInput( uint8_t* keys );

#endif
