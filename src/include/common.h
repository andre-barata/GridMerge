#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <SDL.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

SDL_Color black     = { 0x00, 0x00, 0x00 };
SDL_Color gray1     = { 0x1e, 0x1e, 0x1e };
SDL_Color gray2     = { 0x25, 0x25, 0x26 };
SDL_Color gray3     = { 0x2d, 0x2d, 0x2d };
SDL_Color gray4     = { 0x3c, 0x3c, 0x3c };
SDL_Color white     = { 0xff, 0xff, 0xff };

#include "font.h"

SDL_Window* mainWindow;
SDL_Renderer* windowRenderer;
SDL_Texture *bufferTexture;

bool initWindow(int* width, int* height) {
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL! %s\n", SDL_GetError() );
        return false;
    }
    mainWindow = SDL_CreateWindow("GridMerge", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS );
    if (mainWindow == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating SDL window! %s\n", SDL_GetError() );
        return false;
    }

    // attempt to create a GPU renderer (DirectX, OpenGL, OpenGL ES...), if unsuccessfull create a software renderer
    windowRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if(windowRenderer == NULL) {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to create hardware renderer. Fallback to SOFTWARE rendering... %s\n", SDL_GetError() );
        
        windowRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_SOFTWARE);
        if(windowRenderer == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create software renderer. %s\n", SDL_GetError() );
            return false;
        }
    }

    // attempt to maximize
    int display_index = SDL_GetWindowDisplayIndex(mainWindow);
    if (display_index < 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "failed to get default display index %s\n", SDL_GetError() );
    }
    SDL_Rect usable_bounds;
    if (SDL_GetDisplayUsableBounds(display_index, &usable_bounds) != 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "failed to get display usable bounds %s\n", SDL_GetError() );
    }
    else {
        SDL_SetWindowPosition(mainWindow, usable_bounds.x, usable_bounds.y);
        SDL_SetWindowSize(mainWindow, usable_bounds.w, usable_bounds.h);
    }

    SDL_GetWindowSize(mainWindow, width, height);

    // create a buffer bitmap and a GPU texture from it
    /*
    SDL_Surface* buffer = SDL_CreateRGBSurfaceWithFormat(0, *width, *height, 32, SDL_PIXELFORMAT_RGBA32);
    if (buffer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating buffer texture surface: %s\n", SDL_GetError());
        return false;
    }
	bufferTexture = SDL_CreateTextureFromSurface(windowRenderer, buffer);
	SDL_FreeSurface(buffer);
	if ( bufferTexture == NULL ) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating buffer texture: %s\n", SDL_GetError());
		return false;
	}
    */
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Continuing from window init... \n" );
    return true;
}

int terminate() {
    SDL_StopTextInput();

	TTF_CloseFont(mainFont);
	SDL_DestroyTexture(bufferTexture);
	bufferTexture = NULL;

    SDL_DestroyRenderer(windowRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
    return 1;
}

#endif