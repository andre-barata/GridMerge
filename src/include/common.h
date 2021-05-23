#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <SDL.h>

SDL_Window* mainWindow;
SDL_Renderer* windowrenderer;
TTF_Font* font;
SDL_Texture *bufferTexture, *text;

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

    windowrenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
    if(windowrenderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to get window's surface. %s\n", SDL_GetError() );
        return false;
    }

    SDL_Surface* buffer = SDL_CreateRGBSurfaceWithFormat(0, *width, *height, 32, SDL_PIXELFORMAT_RGBA32);
    if (buffer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating buffer texture surface: %s", SDL_GetError());
        return false;
    }
	bufferTexture = SDL_CreateTextureFromSurface(windowrenderer, buffer);
	SDL_FreeSurface(buffer);
	buffer = NULL;
	if ( bufferTexture == NULL ) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating buffer texture: %s", SDL_GetError());
		return false;
	}

    return true;
}

int terminate() {
    SDL_StopTextInput();

	TTF_CloseFont(font);
	SDL_DestroyTexture(bufferTexture);
	bufferTexture = NULL;

    SDL_DestroyRenderer(windowrenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
    return 1;
}

#endif